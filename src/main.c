#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chess.h"
#include "draw.h"

FILE *inTest, *outTest;
int test, nolog;

struct DataPos cpos = {
.board = {
    [0]  = R|C, N|C, B|C, Q|C, K|C, B|C, N|C, R|C,
    [8]  = P|C, P|C, P|C, P|C, P|C, P|C, P|C, P|C,
    [48] = P, P, P, P, P, P, P, P,
    [56] = R, N, B, Q, K, B, N, R},
.enpas = -1, .kPos = {60, 4}, .castl = {{1, 1}, {1, 1}}, .turn = 0};

void readCoord(int* c) {
    char a, b;
    scanf(" %c%c", &a, &b);
    if (!nolog) fprintf(inTest, "%c%c ", a, b);
    if (a == 'z' && b == 'z') *c = 64;
    else if ('a' <= a && a <= 'h' && '1' <= b && b <= '8') *c = (char)(56 - (b - '1') * 8 + a - 'a');
    else *c = -1;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (!strcmp(argv[1], "test")) {
            test = 1;
            outTest = fopen("test.out", "w");
        }
        else if (!strcmp(argv[1], "nolog")) nolog = 1;
    }
    if (!nolog) inTest = fopen("test.in", "w");
    if (!test) printf("\033[2J");

    unsigned long long selection;
    int pos, npos = -1;
    while (1) {
        selection = 0;
        if (!test) draw(cpos.board, 0, cpos.turn);

        if (npos == -1) {
            if (checkMate(&cpos)) {
                if (checkCheck(cpos.kPos[!!cpos.turn], cpos.turn, cpos.board)) printf("%s HAS WON!\n", (cpos.turn ? "WHITE" : "BLACK"));
                else printf("STALEMATE!\n");
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
        if (!test) draw(cpos.board, selection, cpos.turn);

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
    if (test) for (char *c = cpos.board; c != cpos.board + 64; ++c) fprintf(outTest, "%c", *c);
}
