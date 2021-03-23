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

#ifndef COMMON_H
#define COMMON_H

static const char C       = (1 << 3);
static const char MOD8    = 7;
#define ISSEL(x) !!(selection & (1ULL << (x)))
#define IN(x) (0 <= x && x < 64)

#endif
