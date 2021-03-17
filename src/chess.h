#ifndef CHESS_H
#define CHESS_H

enum {K = 1, Q, R, B, N, P};

struct DataPos {
    char board[64];
    int enpas, kPos[2], castl[2][2], turn, movec, rule50;
};

char checkCheck(int pos, int col, char* board);
char check(int pos, int from, int to, char* board);
unsigned long long possMoves(int pos, struct DataPos* b);
char checkMate(struct DataPos* b);
void move(int from, int to, struct DataPos* b);

#endif
