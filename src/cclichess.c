/*  Copyright (C) 2021 Ignacy Boehlke
    This file is part of cclichess.

    cclichess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cclichess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cclichess.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "common.h"
#include "FEN.h"
#include "chess.h"
#include "draw.h"

FILE *inTest, *outTest;
int test, nolog, reverse, custart;
struct timespec waittime = {.tv_sec = 0};
struct DataPos cpos;

char command;
void readTerm(int* c) {
    char a, b;
    scanf(" %c%c", &a, &b);
    if (!nolog) fprintf(inTest, "%c%c ", a, b);
    if (a == '.') command = b, *c = 64;
    else if ('a' <= a && a <= 'h' && '1' <= b && b <= '8') *c = (char)(56 - (b - '1') * 8 + a - 'a');
    else *c = -1;
}

int main(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "rtqhs:w:v")) != -1) {
        switch (opt) {
        case 'r':
            reverse = C;
            break;
        case 't':
            test = 1;
            outTest = fopen("test.out", "w");
        case 'q':
            nolog = 1;
            break;
        case 's':
            loadFEN(optarg, &cpos);
            custart = 1;
            break;
        case 'w':
            waittime.tv_nsec = atoi(optarg) * 1000000;
            break;
        case 'v':
            printf("cclichess %s\n"
                   "Copyright (C) 2021 Ignacy Boehlke <ignacy.boehlke@protonmail.com>\n"
                   "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n"
                   "This is free software: you are free to change and redistribute it.\n"
                   "There is NO WARRANTY, to the extent permitted by law.\n", VERSION);
            return EXIT_SUCCESS;
        case 'h':
        default:
            puts("usage: chess [options]\n"
                 "\t-h - show this message\n"
                 "\t-r - reverse the chessboard after each move\n"
                 "\t-t - output current selections to test.out and don't print boards\n"
                 "\t-q - don't write input to test.in\n"
                 "\t-w <msecs> - wait msecs miliseconds after each input\n"
                 "\t-s <FEN> - start from a different position\n"
                 "\t-v - version, copyright and license information");
            return EXIT_FAILURE;
        }
    }
    if (!nolog) inTest = fopen("test.in", "w");
    if (!test) printf("\033[2J");
    if (!custart) loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &cpos);

    unsigned long long selection = 0;
    int pos = -1, npos, quit = 0, drawOffered = 0, drawToClaim = 0, redraw = 1, random, gameResult = 0;
    char prom, FEN[90];
    while (1) {
        if (!test && redraw) draw(cpos.board, selection, cpos.turn & reverse);
        if (gameResult) break;
        redraw = 1, random = 0;
        if (waittime.tv_nsec) nanosleep(&waittime, NULL);
        readTerm(&npos);
        if (npos == -1) {
            redraw = 0;
            continue;
        }
        if (npos == 64) {
            switch (command) {
            case 'q':
                quit = 1;
                break;
            case 'o':
                drawOffered = 1;
                break;
            case 'c':
                if (drawOffered || drawToClaim) gameResult = 4, quit = 1;
                break;
            case 'r':
                random = 1;
                for (int i = 0; i < 64; ++i) {
                    if (cpos.board[i] && ((cpos.board[i] & C) == cpos.turn) &&
                            (selection = possMoves(i, &cpos))) {
                        pos = i;
                        npos = __builtin_ctzll(selection);
                        break;
                    }
                }
                break;
            case 'f':
                getFEN(&cpos, FEN);
                printf("%s\n", FEN);
                redraw = 0;
                break;
            case 'h':
            default:
                puts("commands:\n"
                     "\t.q - quit\n"
                     "\t.o - offer draw\n"
                     "\t.c - claim draw\n"
                     "\t.r - first available move (for faster testing)\n"
                     "\t.f - display FEN of the current position\n"
                     "\t.h - display this message");
                redraw = 0;
            }
            if (quit) break;
            if (!random) continue;
        }
        if (ISSEL(npos)) {
            if ((cpos.board[pos] & MOD8) == P && (npos >> 3) == (cpos.turn ? 7 : 0)) {
                if (!random) scanf(" %c", &prom);
                else prom = 'q';
                if (!nolog) fprintf(inTest, "%c ", prom);
                prom = INVLETTERS[prom - 'a'];
                if (!prom) {
                    selection = 0;
                    continue;
                }
            }
            move(pos, npos, &cpos, prom);
            selection = 0;

            if (checkMate(&cpos)) {
                if (checkCheck(cpos.kPos[!!cpos.turn], cpos.turn, cpos.board)) gameResult = !!cpos.turn + 1;
                else gameResult = 3;
            } else if (cpos.rule50 == 150 || cpos.rep == 5) gameResult = 4;
            else if (cpos.rule50 >= 100 || cpos.rep >= 3) drawToClaim = 1;
            else drawToClaim = 0;
            drawOffered = 0;
        } else if (cpos.board[npos] && (cpos.board[npos] & C) == cpos.turn) {
            selection = possMoves(pos = npos, &cpos);
            if (test) fprintf(outTest, "%llu\n", selection);
        } else selection = 0;
    }
    switch (gameResult) {
        case 1:
        case 2:
            printf("%s WON!\n", (gameResult - 1 ? "WHITE" : "BLACK"));
            break;
        case 3:
            puts("STALEMATE!");
            break;
        case 4:
            puts("DRAW!");
    }
    getFEN(&cpos, FEN);
    breakSeq(&cpos);
    if (!nolog) fputc('\n', inTest);
    if (test) fprintf(outTest, "%s\n%d\n", FEN, gameResult);
}
