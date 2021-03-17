#include <stdio.h>
#include "draw.h"

static const char FIGURE[6][7][29] = {
{"            ", "     ▗▖     ", "            ", "            ", "            ", "            ", "            "},
{"            ", "    ▝▜▛▘    ", "  ▖ ▌▐▎▐ ▗  ", "   ▗▖▗▖▗▖   ", "     ▄▄     ", "    ▗▙▄     ", "            "},
{"            ", "  ▟▀▀▜▛▀▀▙  ", "  ▜▖█▐▌█▗▛  ", "   ▝████▘   ", "    ▟█▘▄    ", "   ▟██▙█▄   ", "     ▗▖     "},
{"            ", "  ▜▄ ▐▌ ▄▛  ", "  ▝██████▘  ", "    ████    ", "    █▙▟█    ", "   ████▀█▘  ", "     ██     "},
{"            ", "   ██████   ", "   ██████   ", "   ▗████▖   ", "   ▗▟██▙▖   ", "   ████▙    ", "    ▐██▌    "},
{"            ", "            ", "            ", "            ", "            ", "            ", "            "}
};

#ifdef SAFE
static const char* BG_WH  = "\033[45m";
static const char* BG_BL  = "\033[44m";
static const char* BG_RED = "\033[41m";
static const char* ID_COL = "\033[30m";
static const char* FI_WH  = "\033[37m";
static const char* FI_BL  = "\033[30m";
#else
static const char* BG_WH  = "\033[48;5;245m";
static const char* BG_BL  = "\033[48;5;242m";
static const char* BG_RED = "\033[48;5;9m";
static const char* ID_COL = "\033[38;5;237m";
static const char* FI_WH  = "\033[38;5;251m";
static const char* FI_BL  = "\033[38;5;237m";
#endif

static const char* CL     = "\033[0m";

void draw(char* board, unsigned long long selection, int reversed) {
    reversed = !!reversed;
    int p;
    printf("\033[f");
    for (int row = 0; row <= 56; row += 8) {
        for (int rr = 0; rr < 6; ++rr) {
            for (int pos = row; pos < row + 8; ++pos) {
                p = (reversed ? 63 - pos : pos);
                if (!((pos & 1) ^ ((row >> 3) & 1)) ^ reversed) printf(BG_WH);
                else printf(BG_BL);
                if (ISSEL(p)) printf(BG_RED);
                if (board[p] & C) printf(FI_BL);
                else printf(FI_WH);
                printf("%s%s", FIGURE[rr][board[p] & MOD8], CL);
            }
            if (!((row >> 3) & 1) ^ reversed) printf(BG_WH);
            else printf(BG_BL);
            printf("%s\033[1m %c %s\n", ID_COL, (rr == 2 ? '0' + (reversed ? (row >> 3) + 1 : 8 - (row >> 3)) : ' '), CL);
        }
    }
    printf("\033[1m");
    for (char c = 0; c < 8; ++c) printf("%s%s     %c      ", ((c & 1) ^ reversed ? BG_BL : BG_WH), ID_COL, (reversed ? 'h' - c : 'a' + c));
    printf("%s   %s\n\033[0J", BG_WH, CL);
}
