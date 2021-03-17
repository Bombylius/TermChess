#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "common.h"
#include "FEN.h"
#include "chess.h"
#include "draw.h"

FILE *inTest, *outTest;
int test, nolog, reverse, custart;

struct DataPos cpos;

void readCoord(int* c) {
    char a, b;
    scanf(" %c%c", &a, &b);
    if (!nolog) fprintf(inTest, "%c%c ", a, b);
    if (a == 'z' && b == 'z') *c = 64;
    else if ('a' <= a && a <= 'h' && '1' <= b && b <= '8') *c = (char)(56 - (b - '1') * 8 + a - 'a');
    else *c = -1;
}

int main(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "rtqhs:")) != -1) {
        switch (opt) {
        case 'r':
            reverse = C;
            break;
        case 't':
            test = 1;
            outTest = fopen("test.out", "w");
            break;
        case 'q':
            nolog = 1;
            break;
        case 's':
            loadFEN(optarg, &cpos);
            custart = 1;
            break;
        case 'h':
        default:
            puts("usage: chess [options]\n"
                 "\th - show this message\n"
                 "\tr - reverse the chessboard after each move\n"
                 "\tt - output current selections to test.out and don't print boards\n"
                 "\tq - don't write input to test.in\n"
                 "\ts <FEN> - start from a different position");
            return EXIT_FAILURE;
        }
    }
    if (!nolog) inTest = fopen("test.in", "w");
    if (!test) printf("\033[2J");
    if (!custart) loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", &cpos);

    unsigned long long selection;
    int pos, npos = -1;
    while (1) {
        selection = 0;
        if (!test) draw(cpos.board, 0, cpos.turn & reverse);

        if (npos == -1) {
            if (checkMate(&cpos)) {
                if (checkCheck(cpos.kPos[!!cpos.turn], cpos.turn, cpos.board)) printf("%s HAS WON!\n", (cpos.turn ? "WHITE" : "BLACK"));
                else puts("STALEMATE!");
                break;
            }
            if (cpos.rule50 == 100) {
                puts("DRAW BY 50 MOVE RULE");
                break;
            }
            readCoord(&pos);
            while ((!IN(pos) || (cpos.board[pos] & C) != cpos.turn || !cpos.board[pos]) && pos != 64) {
                puts("Wrong selection.");
                readCoord(&pos);
            }
            if (pos == 64) break;
        } else pos = npos;

        selection = possMoves(pos, &cpos);
        if (test) fprintf(outTest, "%llu\n", selection);
        if (!test) draw(cpos.board, selection, cpos.turn & reverse);

        readCoord(&npos);
        if (npos == 64) break;
        if (!ISSEL(npos)) {
            if (!(cpos.board[npos] && (cpos.board[npos] & C) == cpos.turn)) npos = -1;
            continue;
        }
        move(pos, npos, &cpos);

        cpos.turn ^= C;
        npos = -1;
    }
    char FEN[80];
    getFEN(&cpos, FEN);
    if (test) fprintf(outTest, "%s\n", FEN);
}
