#ifndef FEN_H
#define FEN_H
#include "chess.h"

void getFEN(struct DataPos* pos, char* FEN);
void loadFEN(char* FEN, struct DataPos* pos);

#endif
