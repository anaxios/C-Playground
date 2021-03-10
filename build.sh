#/usr/bin/env sh

gcc -std=c11 -Wall -Wextra -pedantic -ggdb -o latin-square scratch.c -lgcrypt
