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

#ifndef CHESS_H
#define CHESS_H

#include "common.h"

enum {K = 1, Q, R, B, N, P};
enum {UNDECIDED, WHITE_WON, BLACK_WON, STALEMATE, DRAW};
static const char LETTERS[7] = {
    [K] = 'k',
    [Q] = 'q',
    [R] = 'r',
    [B] = 'b',
    [N] = 'n',
    [P] = 'p',
};
static const char INVLETTERS[26] = {
    ['k' - 'a'] = K,
    ['q' - 'a'] = Q,
    ['r' - 'a'] = R,
    ['b' - 'a'] = B,
    ['n' - 'a'] = N,
    ['p' - 'a'] = P,
};

struct histPos {
    char c;
    char pos[69];
    struct histPos* next;
};

#define HASH_SIZE 307
struct DataPos {
    char board[64], pHis[400][8];
    struct histPos *hisHashes[HASH_SIZE], *lastPos[151];
    int enpas, kPos[2], castl[2][2], turn, movec, rule50, rep, posC, lastHash[151];
};

int hashPos(struct DataPos* b);
int cmpHPos(char* HPos, struct DataPos* b);
struct histPos* addHis(struct DataPos* b);
void breakSeq(struct DataPos* b);
char checkCheck(int pos, int col, char* board);
char check(int pos, int from, int to, char* board);
u64 possMoves(int pos, struct DataPos* b);
char checkMate(struct DataPos* b);
void move(int from, int to, struct DataPos* b, char prom);

#endif
