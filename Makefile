CFLAGS = -std=c99 -Wall -Wshadow -Wextra -Wconversion -pedantic

SRC = main.c chess.c draw.c FEN.c
OBJ := $(SRC:.c=.o)

all: CFLAGS += -O2
all: chess

safe: CFLAGS += -DSAFE
safe: all

debug: CFLAGS += -ggdb3 #-fsanitize=address,undefined
debug: chess

VPATH=src
FEN.o: FEN.c FEN.h chess.h common.h
chess.o: chess.c chess.h common.h
draw.o: draw.c draw.h common.h
main.o: draw.h chess.h common.h FEN.h

$(OBJ): Makefile
chess: $(OBJ)

clean:
	rm -f $(OBJ) test.in test.out
