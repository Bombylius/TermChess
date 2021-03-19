#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chess.h"

static const char IMOD8   = 0170;
#define IN(x) (0 <= x && x < 64)
#define INL(x) (0 <= x && x < 8)
#define CANGO(x, c) ((b->board[npos = x] & C) != c || !b->board[npos])
#define CANG(x, c) ((b->board[x] & C) != c || !b->board[x])
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

void move(int from, int to, struct DataPos* b, char prom) {
    int y = from >> 3, x = from & MOD8, type = b->board[from] & MOD8, tmp;
    if (b->board[from] & C) ++b->movec;
    if (b->board[to]) b->rule50 = -1;

    if (to == 0 || to == 7 || to == 56 || to == 63) b->castl[!(to >> 3)][!!(to & MOD8)] = 0;

    switch (type) {
    case P:
        b->rule50 = -1;
        if (abs(to - from) == 16) {
            b->enpas = (to + from) >> 1;
            break;
        }
        else b->enpas = -1;
        if ((tmp = (to & MOD8) - x) && !b->board[to]) b->board[from + tmp] = 0;
        else if ((y == (b->turn ? 6 : 1))) b->board[from] = (b->board[from] & IMOD8) | prom;
        break;
    case K:
        b->kPos[!!b->turn] = to;
        b->castl[!!b->turn][0] = b->castl[!!b->turn][1] = 0;
        if (abs(tmp = to - from) == 2) {
            if (tmp > 0) b->board[to - 1] = b->board[y * 8 + 7], b->board[y * 8 + 7] = 0;
            else b->board[to + 1] = b->board[y * 8], b->board[y * 8] = 0;
        }
        break;
    case R:
        if ((!x || x == 7) && y == (b->board[from] & C ? 0 : 7)) b->castl[!!b->turn][!!x] = 0;
    }
    if (type != P) b->enpas = -1;
    b->board[to] = b->board[from];
    b->board[from] = 0;
    ++b->rule50;
}

// We don't have to simulate the exact moves, because you can't get checked from behind the rook when castling
// from behind the captured pawn when capturing en passant and it doesn't matter what you promote your pawn into
char check(int pos, int from, int to, char* board) {
    char beat = board[to];
    board[to] = board[from];
    board[from] = 0;

    char res = checkCheck(pos, board[to] & C, board);

    board[from] = board[to];
    board[to] = beat;
    return res;
}

#define EXP {\
    if (b->board[npos]) {\
        if ((b->board[npos] & C) != b->turn)\
            SEL(npos);\
        break;\
    }\
    SEL(npos);\
}

unsigned long long possMoves(int pos, struct DataPos* b) {
    int x = pos & MOD8, y = pos >> 3, npos, sid, type, tmp;
    unsigned long long selection = 0;
    switch (type = b->board[pos] & MOD8) {
    case K:
        for (int a = -1; a <= 1; ++a) for (int bb = -1; bb <= 1; ++bb)
            if (INL(x + a) && INL(y + bb) && a | bb && CANGO(pos + bb * 8 + a, b->turn)) SEL(npos);

        for (int j = 0; j <= 1; ++j) {
            sid = (j ? -1 : 1);
            if (b->castl[!!b->turn][!j] && b->board[pos + (3 + j) * sid] == (R | b->turn)) { //xd
                tmp = 1;
                for (int i = 1; i < 3 + j; ++i) if (b->board[pos + i * sid]) {
                    tmp = 0;
                    break;
                }
                if (tmp) {
                    for (int i = 0; i < 3; ++i) if (checkCheck(pos + i * sid, b->turn, b->board)) {
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
            if (INL(x + i) && INL(y + j) && CANGO(pos + j * 8 + i, b->turn)) SEL(npos);
            if (INL(x + j) && INL(y + i) && CANGO(pos + i * 8 + j, b->turn)) SEL(npos);
        }
        break;
    case P:
        sid = (b->turn ? 1 : -1);
        if (!b->board[npos = pos + 8 * sid]) {
            SEL(npos);
            if (y == (b->turn ? 1 : 6) && !b->board[npos = pos + 16 * sid]) SEL(npos);
        }
        npos = pos - 1 + sid * 8;
        if (x && ((npos == b->enpas && (b->board[pos - 1] & C) != b->turn) ||
                 (b->board[npos] && (b->board[npos] & C) != b->turn))) SEL(npos);
        npos += 2;
        if (x != 7 && ((npos == b->enpas && (b->board[pos + 1] & C) != b->turn) ||
                      (b->board[npos] && (b->board[npos] & C) != b->turn))) SEL(npos);
    }
    for (int i = 0; i < 64; ++i)
        if (ISSEL(i) && check(type == K ? i : b->kPos[!!b->turn], pos, i, b->board)) SSEL(i);
    return selection;
}

char checkMate(struct DataPos* b) {
    for (int i = 0; i < 64; ++i) if ((b->board[i] & C) == b->turn && possMoves(i, b)) return 0;
    return 1;
}
