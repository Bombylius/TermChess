/*  Copyright (C) 2021 Ignacy Boehlke
    This file is part of TermChess.

    TermChess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TermChess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TermChess.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>
#include "notation.h"
#include "chess.h"
#include "common.h"

void getPGN(char* str, char his[][8], int result, char* startPos) {
    int i = -1, k = 0;
    while (!his[++i][0]) {};
    char res[8];
    if (!result) sprintf(res, "*");
    else if (result <= 2) sprintf(res, "%d-%d", !!(result - 1), !(result - 1));
    else sprintf(res, "1/2-1/2");

    k += sprintf(str + k, "[Result \"%s\"]\n", res);
    if (startPos) k += sprintf(str + k, "[FEN \"%s\"]\n[SetUp \"1\"]\n", startPos);
    k += sprintf(str + k, "\n");
    if (i & 1) k += sprintf(str + k, "%d. ... %s ", (i >> 1) + 1, his[i]), ++i;
    for (; i < 400 && his[i][0]; i += 2)
        k += sprintf(str + k, "%d. %s %s ", (i >> 1) + 1, his[i], his[i + 1]);

    if (!result) return;
    if (!his[i - 1][0]) --k;
    if (result <= 2) str[k - 2] = '#';
    sprintf(str + k, "%s", res);
}

void getFEN(struct DataPos* pos, char* FEN) {
    int s = 0, empty = 0;
    for (int i = 0; i < 64; ++i) {
        if (!(i & MOD8) && i) FEN[s++] = '/';
        if (!pos->board[i]) ++empty;
        if (empty && (pos->board[i] || (i & MOD8) == 7)) FEN[s++] = '0' + (char)empty, empty = 0;
        if (pos->board[i])
            FEN[s++] = LETTERS[pos->board[i] & MOD8] + !(pos->board[i] & C) * ('A' - 'a');
    }
    FEN[s] = '\0';
    char castl[5];
    s = 0;
    if (pos->castl[0][1]) castl[s++] = 'K';
    if (pos->castl[0][0]) castl[s++] = 'Q';
    if (pos->castl[1][1]) castl[s++] = 'k';
    if (pos->castl[1][0]) castl[s++] = 'q';
    if (s == 0) memcpy(castl, "-", 2);
    else castl[s] = '\0';

    char enpas[3] = "";
    if (pos->enpas == -1) memcpy(enpas, "-", 2);
    else sprintf(enpas, "%c%d", 'a' + (pos->enpas & MOD8), 8 - (pos->enpas >> 3));

    sprintf(FEN, "%s %c %s %s %d %d", FEN, (pos->turn ? 'b' : 'w'),
            castl, enpas, pos->rule50, pos->movec);
}

void loadFEN(char* FEN, struct DataPos* pos) {   
    memset(pos->board, 0, 64);
    memset(pos->pHis, '\0', sizeof pos->pHis);
    for (int i = 0; i < HASH_SIZE; ++i) pos->hisHashes[i] = NULL;
    pos->posC = 0;
    pos->rep = 1;
    int s = 0;
    for (int i = 0; i < 64; ++i) {
        if (FEN[s] == '/') ++s;
        if ('0' < FEN[s] && FEN[s] < '9') {
            memset(pos->board + i, 0, (unsigned long)(FEN[s] - '0'));
            i += FEN[s] - '1';
        } else {
            if (FEN[s] == 'k' || FEN[s] == 'K') pos->kPos[FEN[s] > 'a'] = i;
            if (FEN[s] < 'a') pos->board[i] = INVLETTERS[FEN[s] - 'A'];
            else pos->board[i] = INVLETTERS[FEN[s] - 'a'] | C;
        }
        ++s;
    }
    pos->turn = (FEN[++s] == 'b') * C;
    ++s;
    
    if (FEN[++s] == 'K') pos->castl[0][1] = 1, ++s;
    if (FEN[s] == 'Q') pos->castl[0][0] = 1, ++s;
    if (FEN[s] == 'k') pos->castl[1][1] = 1, ++s;
    if (FEN[s] == 'q') pos->castl[1][0] = 1, ++s;
    if (FEN[s] == '-') ++s;
    ++s;

    if (FEN[s] != '-') pos->enpas = FEN[s] - 'a' + (8 - FEN[s + 1] + '0') * 8, s += 2;
    else pos->enpas = -1, ++s;
    sscanf(FEN + s, "%d%d", &pos->rule50, &pos->movec);
    if (pos->enpas == -1) ++addHis(pos)->c;
}
