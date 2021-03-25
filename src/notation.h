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

#ifndef FEN_H
#define FEN_H
#include "chess.h"

void getPGN(char* str, char his[][8], int result, char* startPos);
void getFEN(struct DataPos* pos, char* FEN);
void loadFEN(char* FEN, struct DataPos* pos);

#endif
