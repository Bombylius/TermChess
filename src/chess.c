#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chess.h"

static const char IMOD8   = 0170;
#define IN(x) (0 <= x && x < 64)
#define INL(x) (0 <= x && x < 8)
#define CANGO(x, c) ((board->board[npos = x] & C) != c || !board->board[npos])
#define CANG(x, c) ((board->board[x] & C) != c || !board->board[x])
#define MIN(a, b) (a < b ? a : b)
#define SEL(x) selection |= (1ULL << (x))
#define SSEL(x) selection ^= (1ULL << (x))

#define CEXP(x) if(board[npos]) {\
    if ((board[npos] & C) != col && ((board[npos] & MOD8) == x || (board[npos] & MOD8) == Q)) return 1;\
    break;\
}

char checkCheck(int pos, int col, char* board) {
    int y = pos >> 3, x = pos & MOD8, npos, sid;

    for (int a = -1; a <= 1; ++a) for (int b = -1; b <= 1; ++b)
        if (INL(x + a) && INL(y + b) && a | b && (board[npos = pos + b * 8 + a] & MOD8) == K &&
                (board[npos] & C) != col) return 1;

    for (npos = pos - 1; npos >= y * 8; --npos) CEXP(R)
    for (npos = pos + 1; npos < (y + 1) * 8; ++npos) CEXP(R)
    for (npos = pos + 8; IN(npos); npos += 8) CEXP(R)
    for (npos = pos - 8; IN(npos); npos -= 8) CEXP(R)

    npos = pos - 9; for (int i = 0; i < MIN(x, y); ++i, npos -= 9) CEXP(B)
    npos = pos - 7; for (int i = 0; i < MIN(7 - x, y); ++i, npos -= 7) CEXP(B)
    npos = pos + 7; for (int i = 0; i < MIN(x, 7 - y); ++i, npos += 7) CEXP(B)
    npos = pos + 9; for (int i = 0; i < MIN(7 - x, 7 - y); ++i, npos += 9) CEXP(B)

    for (int i = -1; i <= 1; i += 2) for (int j = -2; j <= 2; j += 4) {
        if (INL(x + i) && INL(y + j) && (board[npos = pos + j * 8 + i] & C) != col && (board[npos] & MOD8) == N)
            return 1;
        if (INL(x + j) && INL(y + i) && (board[npos = pos + i * 8 + j] & C) != col && (board[npos] & MOD8) == N)
            return 1;
    }

    sid = (col ? 1 : -1);
    for (int i = -1; i <= 1; i += 2)
        if (x != (i == 1 ? 7 : 1) && y != (col ? 7 : 0) &&
                (board[npos = pos + sid * 8 + i] & C) != col && (board[npos] & MOD8) == P) return 1;

    return 0;
}

void move(int from, int to, struct DataPos* board) {
    int y = from >> 3, x = from & MOD8, type = board->board[from] & MOD8, tmp;

    switch (type) {
    case P:
        if (abs(to - from) == 16) {
            board->enpas = to;
            break;
        }
        else board->enpas = -1;
        if ((tmp = (to & MOD8) - x) && !board->board[to]) board->board[from + tmp] = 0;
        else if ((y == (board->turn ? 6 : 1))) {
            char c;
            do {
                printf("What do you want to transform into? [Q/R/B/N]\n");
                scanf(" %c", &c);
                //if (!nolog) fprintf(inTest, "%c ", c);
                switch (c) {
                case 'Q': c = Q; break;
                case 'R': c = R; break;
                case 'B': c = B; break;
                case 'N': c = N; break;
                default: c = 0;
                }
            } while (!c);
            board->board[from] = (board->board[from] & IMOD8) | c;
        }
        break;
    case K:
        board->kPos[!!board->turn] = to;
        board->castl[!!board->turn][0] = board->castl[!!board->turn][1] = 0;
        if (abs(tmp = to - from) == 2) {
            if (tmp > 0) board->board[to - 1] = board->board[y * 8 + 7], board->board[y * 8 + 7] = 0;
            else board->board[to + 1] = board->board[y * 8], board->board[y * 8] = 0;
        }
        break;
    case R:
        if (!x || x == 7) board->castl[!!board->turn][!!x] = 0;
    }
    if (type != P) board->enpas = -1;
    board->board[to] = board->board[from];
    board->board[from] = 0;
}

char aux[64];
char check(int pos, int from, int to, char* board) {
    memcpy(aux, board, 64);

    int tmp = aux[to];
    aux[to] = aux[from];
    aux[from] = 0;

    int x = pos & MOD8, type = (aux[to] & MOD8);
    if (type == P && !tmp && (tmp = (to & MOD8) - x)) aux[pos + tmp] = 0;

    return checkCheck(pos, aux[to] & C, aux);
}

#define EXP {\
    if (board->board[npos]) {\
        if ((board->board[npos] & C) != board->turn)\
            SEL(npos);\
        break;\
    }\
    SEL(npos);\
}

unsigned long long possMoves(int pos, struct DataPos* board) {
    int x = pos & MOD8, y = pos >> 3, npos, sid, type, tmp;
    unsigned long long selection = 0;
    switch (type = board->board[pos] & MOD8) {
    case K:
        for (int a = -1; a <= 1; ++a) for (int b = -1; b <= 1; ++b)
            if (INL(x + a) && INL(y + b) && a | b && CANGO(pos + b * 8 + a, board->turn)) SEL(npos);

        for (int j = 0; j <= 1; ++j) {
            sid = (j ? -1 : 1);
            if (board->castl[!!board->turn][!j] && board->board[pos + (3 + j) * sid] == (R | board->turn)) {
                tmp = 1;
                for (int i = 1; i < 3 + j; ++i) if (board->board[pos + i * sid]) {
                    tmp = 0;
                    break;
                }
                if (tmp) {
                    for (int i = 0; i < 3; ++i) if (checkCheck(pos + i * sid, board->turn, board->board)) {
                        tmp = 0;
                        break;
                    }
                    if (tmp) SEL(pos + 2 * sid);
                }
            }
        }

        break;
    case Q:
    case R:
        for (npos = pos - 1; npos >= y * 8; --npos) EXP
        for (npos = pos + 1; npos < (y + 1) * 8; ++npos) EXP
        for (npos = pos + 8; IN(npos); npos += 8) EXP
        for (npos = pos - 8; IN(npos); npos -= 8) EXP

        if (type == R) break;
        __attribute__ ((fallthrough)); // suppresing gcc warning
    case B:
        npos = pos - 9; for (int i = 0; i < MIN(x, y); ++i, npos -= 9) EXP
        npos = pos - 7; for (int i = 0; i < MIN(7 - x, y); ++i, npos -= 7) EXP
        npos = pos + 7; for (int i = 0; i < MIN(x, 7 - y); ++i, npos += 7) EXP
        npos = pos + 9; for (int i = 0; i < MIN(7 - x, 7 - y); ++i, npos += 9) EXP
        break;
    case N:
        for (int i = -1; i <= 1; i += 2) for (int j = -2; j <= 2; j += 4) {
            if (INL(x + i) && INL(y + j) && CANGO(pos + j * 8 + i, board->turn)) SEL(npos);
            if (INL(x + j) && INL(y + i) && CANGO(pos + i * 8 + j, board->turn)) SEL(npos);
        }
        break;
    case P:
        sid = (board->turn ? 1 : -1);
        if (!board->board[npos = pos + 8 * sid]) {
            SEL(npos);
            if (y == (board->turn ? 1 : 6) && !board->board[npos = pos + 16 * sid]) SEL(npos);
        }
        npos = pos - 1 + sid * 8;
        if (x && ((pos - 1 == board->enpas && board->board[pos - 1] && (board->board[pos - 1] & C) != board->turn) ||
                    (board->board[npos] && (board->board[npos] & C) != board->turn))) SEL(npos);
        npos += 2;
        if (x != 7 && ((pos + 1 == board->enpas && board->board[pos + 1] && (board->board[pos + 1] & C) != board->turn) ||
                    (board->board[npos] && (board->board[npos] & C) != board->turn))) SEL(npos);
    }
    for (int i = 0; i < 64; ++i) if (ISSEL(i) && check(type == K ? i : board->kPos[!!board->turn], pos, i, board->board)) SSEL(i);
    return selection;
}

char checkMate(struct DataPos* board) {
    for (int i = 0; i < 64; ++i) if ((board->board[i] & C) == board->turn && possMoves(i, board)) return 0;
    return 1;
}
