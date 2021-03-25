CC = clang
CFLAGS = -std=gnu99 -Wall -Wshadow -Wextra -Wconversion -pedantic -DVERSION=\"0.2\"

SRC = cclichess.c chess.c draw.c notation.c
OBJ := $(SRC:.c=.o)

all: CFLAGS += -O2
all: cclichess

safe: CFLAGS += -DSAFE
safe: all

debug: CFLAGS += -ggdb3 -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: cclichess

VPATH=src
FEN.o: notation.c notation.h chess.h common.h
chess.o: chess.c chess.h common.h
draw.o: draw.c draw.h common.h
main.o: draw.h chess.h common.h notation.h

$(OBJ): Makefile
cclichess: $(OBJ)

clean:
	rm -f $(OBJ) test.in test.out
