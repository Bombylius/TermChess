#ifndef COMMON_H
#define COMMON_H

static const char C       = (1 << 3);
static const char MOD8    = 7;
#define ISSEL(x) !!(selection & (1ULL << (x)))
#define IN(x) (0 <= x && x < 64)

#endif
