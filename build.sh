#/usr/bin/env sh

gcc -O3 -std=c11 -Wall -Wextra -pedantic -ggdb -o latin-square scratch.c -lgcrypt
