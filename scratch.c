#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

typedef struct {
  size_t count;
  const char *data;
} String_View;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int) sv.count, sv.data
#define SV(cstr_lit) \
    ((String_View) { \
        .count = sizeof(cstr_lit) - 1, \
        .data = (cstr_lit) \
    })

#define SV_NULL (String_View) {0}

typedef struct {
  size_t row;
  size_t col;
  char elem;
} Cell;

typedef struct {
  String_View key;
  size_t size;
  Cell *array;
} Latin_Square;

size_t exponent(size_t n)
{
  size_t temp = 1;
  for (size_t i = 0; i < n; i++) {
    temp += n * n;
  }
  return temp;
}

/* Latin_Square switch_columns(Latin_Sqaure square,size_t col1, size_t col2) */
/* { */
/*   return */
/* } */

bool is_latin_square(Latin_Square *square)
{
  for (size_t to_match = 0;to_match < square->size; to_match++) {
    for (size_t to_search = 0;to_search < square->size; to_search++) {
      if ((square->array[to_match].row == square->array[to_search].row
	   && square->array[to_match].elem == square->array[to_search].elem)
	  &&
	  (square->array[to_match].col == square->array[to_search].col
	   && square->array[to_match].elem == square->array[to_search].elem))
	continue;
	  
      if ((square->array[to_match].col == square->array[to_search].col
	   && square->array[to_match].elem == square->array[to_search].elem)
	  ||
	  (square->array[to_match].row == square->array[to_search].row
	   && square->array[to_match].elem == square->array[to_search].elem))
	return false;
      
    }
  }
  
  return true;
}

Latin_Square make_latin_square(String_View key)
{
  Latin_Square square;
  square.key = key;
  square.size = key.count * key.count;
  square.array = (Cell *)malloc(square.size * sizeof(Cell));
  assert(square.array != NULL);
  size_t r = -1;
  //  printf("%lu", square.key);
  for (size_t tuple = 0; tuple < square.size; tuple++) {
    if (tuple % key.count == 0) r++;
    square.array[tuple] = (Cell) { .row = r,
			       .col = (tuple % key.count),
			       .elem = key.data[(r + tuple) % key.count]};
  }

  return square;
}

void print_latin_square(Latin_Square *square)
{
  //size_t s = sizeof(square->square) / sizeof  square->square[0];
  for (size_t tuple = 0; tuple < square->size; tuple++)  {
    printf("%c ", square->array[tuple].elem);
    if (square->array[tuple].col == (square->key.count - 1)) printf("\n"); 
  }

}

int main(int argc, char **argv)
{
  
  String_View key = SV("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  Latin_Square a = make_latin_square(key);

  print_latin_square(&a);
  
  (is_latin_square(&a))
    ? printf("This is a latin square.\n")
    : printf("This is NOT a latin square.\n");
  
  free(a.array);
  return 0;
}
