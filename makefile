CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -ggdb # -D_REENTRANT

# Simple2D Dependency
#S2DPATH = /usr/include/SDL2
LIBS = -lgcrypt # -lsimple2d -lSDL2 -lGL -lm -lSDL2_image -lSDL2_mixer -lSDL2_ttf

all: build

# ----- Build Source ------ #

build:
	$(CC) $(CFLAGS) scratch.c -o latin-square $(LIBS) # -I$(S2DPATH) 
