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
    while ((opt = getopt(argc, argv, "rtqhs:w:")) != -1) {
        switch (opt) {
        case 'r':
            reverse = C;
            break;
        case 't':
            test = 1;
            outTest = fopen("test.out", "w");
            __attribute__ ((fallthrough)); // suppresing gcc warning
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
        case 'h':
        default:
            puts("usage: chess [options]\n"
                 "\th - show this message\n"
                 "\tr - reverse the chessboard after each move\n"
                 "\tt - output current selections to test.out and don't print boards\n"
                 "\tq - don't write input to test.in\n"
                 "\tw <msecs> - wait msecs miliseconds after each input\n"
                 "\ts <FEN> - start from a different position");
            return EXIT_FAILURE;
        }
    }
    if (!nolog) inTest = fopen("test.in", "w");
    if (!test) printf("\033[2J");
    if (!custart) loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &cpos);

    unsigned long long selection = 0;
    int pos = -1, npos, quit = 0, drawOffered = 0, drawToClaim = 0, redraw = 1, random;
    char prom;
    while (1) {
        if (!test && redraw) draw(cpos.board, selection, cpos.turn & reverse);
        redraw = 1, random = 0;
        if (waittime.tv_nsec) nanosleep(&waittime, NULL);
        readTerm(&npos);
        if (npos == 64) {
            switch (command) {
            case 'q':
                quit = 1;
                break;
            case 'o':
                drawOffered = 1;
                break;
            case 'c':
                if (drawOffered || drawToClaim) puts("DRAW"), quit = 1;
                break;
            case 'r':
                random = 1;
                for (int i = 0; i < 64; ++i) {
                    if (cpos.board[i] && ((cpos.board[i] & C) == cpos.turn) &&
                            (selection = possMoves(i, &cpos))) {
                        pos = i;
                        for (int j = 0; j < 64; ++j) if ((1ULL << j) & selection) {
                            npos = j;
                            break;
                        }
                        break;
                    }
                }
                break;
            case 'x':
                printf("xd: %d\n", checkMate(&cpos));
                redraw = 0;
                break;
            case 'h':
            default:
                puts("commands:\n"
                     "\t.q - quit\n"
                     "\t.o - offer draw\n"
                     "\t.c - claim draw\n"
                     "\t.r - first available move (for faster testing)\n"
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
            cpos.turn ^= C;
            selection = 0;

            if (checkMate(&cpos)) {
                if (checkCheck(cpos.kPos[!!cpos.turn], cpos.turn, cpos.board))
                    printf("%s WON!\n", (cpos.turn ? "WHITE" : "BLACK"));
                else puts("STALEMATE!");
                break;
            }
            if (cpos.rule50 == 150) {
                puts("DRAW");
                break;
            }
            if (cpos.rule50 >= 100) drawToClaim = 1;
            else drawToClaim = 0;
            drawOffered = 0;
        } else if (cpos.board[npos] && (cpos.board[npos] & C) == cpos.turn) {
            selection = possMoves(pos = npos, &cpos);
            if (test) fprintf(outTest, "%llu\n", selection);
        } else selection = 0;
    }

    char FEN[90];
    getFEN(&cpos, FEN);
    if (!nolog) fputc('\n', inTest);
    if (test) fprintf(outTest, "%s\n", FEN);
}
