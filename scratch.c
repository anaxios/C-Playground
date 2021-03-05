#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <gcrypt.h>

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

void swap_cells(Latin_Square *square, char elem1, char elem2)
{
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].elem == elem1) {
      square->array[tuple].elem = elem2;
    /*   continue; */
    } else if (square->array[tuple].elem == elem2) {
      square->array[tuple].elem = elem1;
    /*   continue; */
    }
  }
}

/* void swap_cols(Latin_Square *square, size_t col1, size_t col2) */
/* { */
/*   for (size_t tuple = 0; tuple < square->size; tuple++) { */
/*     if (square->array[tuple].col == col1) { */
/*       square->array[tuple].col = '0'; */
/*       /\*   continue; *\/ */
/*     } else if (square->array[tuple].col == col2) { */
/*       square->array[tuple].col = col1; */
/*       /\*   continue; *\/ */
/*     }     */
/*   } */
/* } */

void swap_rows(Latin_Square *square, size_t row1, size_t row2)
{
  Cell *temp_row1 = (Cell*)malloc(square->key.count * sizeof(Cell));
  Cell *temp_row2 = (Cell*)malloc(square->key.count * sizeof(Cell));
  
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].row == row1)
      temp_row1[tuple % square->key.count] = square->array[tuple];
    if (square->array[tuple].row == row2)
      temp_row2[tuple % square->key.count] = square->array[tuple];
  }

  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].row == row1) {
      square->array[tuple] = temp_row2[tuple % square->key.count];
      square->array[tuple].row = row1;
    } else if (square->array[tuple].row == row2) {
      square->array[tuple] = temp_row1[tuple % square->key.count];
      square->array[tuple].row = row2;
    }
  }

    //  for (size_t i = 0; i < (square->key.count); i++) {
    //printf("%lu-%lu-%c\n", temp[i].row, temp[i].col, temp[i].elem);}

  free(temp_row1);
  free(temp_row2);
}

void swap_cols(Latin_Square *square, size_t col1, size_t col2)
{
  Cell *temp_col1 = (Cell*)malloc(square->key.count * sizeof(Cell));
  Cell *temp_col2 = (Cell*)malloc(square->key.count * sizeof(Cell));
  size_t temp_1 = 0;
  size_t temp_2 = 0;
  
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].col == col1) 
      temp_col1[temp_1++] = square->array[tuple];
    if (square->array[tuple].col == col2)
      temp_col2[temp_2++] = square->array[tuple];
    //   printf(" %lu\n", tuple % square->key.count);
  }

  temp_1 = 0;
  temp_2 = 0;
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].col == col1) {
      square->array[tuple] = temp_col2[temp_2++];
      square->array[tuple].col = col1;
    } else if (square->array[tuple].col == col2) {
      square->array[tuple] = temp_col1[temp_1++];
      square->array[tuple].col = col2;
    }
  }

  /* for (size_t i = 0; i < (square->key.count); i++) { */
  /*   printf("%lu-%lu-%c\n", temp_col1[i].row, temp_col1[i].col, temp_col1[i].elem);} */
  /*   for (size_t i = 0; i < (square->key.count); i++) { */
  /*   printf("%lu-%lu-%c\n", temp_col2[i].row, temp_col2[i].col, temp_col2[i].elem);} */

  free(temp_col1);
  free(temp_col2);
}

void randomize_square(Latin_Square *square)
{
  time_t t;
  srand((unsigned) time(&t));
  for (size_t i = 0; i < 1000000; i++) {
    int r = rand() % 3;
    if ( r == 0 ) {
      swap_cells(square, (rand() % square->key.count), (rand() % square->key.count));
    } else if ( r == 1 ) {
      swap_rows(square, (rand() % square->key.count), (rand() % square->key.count));
    } else {
      swap_cols(square, (rand() % square->key.count), (rand() % square->key.count));
    }
    
  }
  //printf("\n%d\n", rand());
}

typedef struct {
  size_t count;
  const unsigned char *data;
} Hash_View;

unsigned char* hash(Hash_View *key, unsigned char *buffer)
{
  /* for (size_t i = 0; i < key->count; i++) { */
  /*   printf("%02x", key->data[i]); /\* print the result *\/ */
  /* } */
  //  printf("\n");
  unsigned int l = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  gcry_md_hd_t h;
  gcry_md_open(&h, GCRY_MD_SHA512, GCRY_MD_FLAG_SECURE);
  //gcry_md_enable(h, GCRY_MD_SHA512);
  gcry_md_write(h, key->data, key->count);
  unsigned char *x = gcry_md_read(h, GCRY_MD_SHA512);
  //gcry_md_close(h);
  for (size_t i = 0; i < l; i++) {
    //   printf("%02x", x[i]); /* print the result */
    buffer[i] = x[i];
  }
  //  printf("\n");
  gcry_md_close(h);
  return buffer;
}



void encode_square(Latin_Square *square, String_View *key)
{
  unsigned int l = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  
  unsigned char *buffer = (unsigned char*)malloc(l);
  Hash_View x;
  x.data = (unsigned char*)key->data;
  x.count = key->count;
  x.data = hash(&x, buffer);
  x.count = sizeof(x.data);

  
  size_t n = 0;
  while (n < 1 ) {
    x.data = hash(&x, buffer);
    /* for (size_t i = 0; i < l; i++) { */
    /*   printf("%02x", x.data[i]); /\* print the result *\/ */

    /* } */
    /* printf("\n\n"); */

    for (size_t index = 0; index < (l - 1); index++) {
      if (x.data[index] % 3 == 0) {
      	swap_cells(square,
      		   square->key.data[x.data[index] % square->key.count],
      		   square->key.data[x.data[index + 1] % square->key.count]);
      } else if  (x.data[index] % 3 == 1) {
	swap_rows(square,
		  x.data[index] % square->key.count,
		  x.data[index + 1] % square->key.count);
      } else {
	swap_rows(square,
		  x.data[index] % square->key.count,
		  x.data[index + 1] % square->key.count);
      }
    }
    
    n++;
  }
  free(buffer);
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

void print_latin_square(Latin_Square *square, size_t flag)
{
  if (flag == 0) {
    for (size_t tuple = 0; tuple < square->size; tuple++)  {
      printf("%c ", square->array[tuple].elem);
    if (square->array[tuple].col == (square->key.count - 1)) printf("\n");
    }
  } else {
  //size_t s = sizeof(square->square) / sizeof  square->square[0];
    for (size_t tuple = 0; tuple < square->size; tuple++)  {
      printf("|%lu,%lu%c|",
	     square->array[tuple].row,
	     square->array[tuple].col,
	     square->array[tuple].elem);
      if (square->array[tuple].col == (square->key.count - 1)) printf("\n"); 
    }
  }
}

int main(int argc, char **argv)
{
  
  String_View seed = SV("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  String_View key = SV("monkey123");
  Latin_Square a = make_latin_square(seed);

  // swap_cells(&a, 'A', 'Z');
  // swap_cols(&a, 1, 5);
  // swap_rows(&a, 4, 3);
  // swap_rows(&a, 3, 4);
  //randomize_square(&a);
   encode_square(&a, &key);
  // printf("\n");
  print_latin_square(&a, 0);

  (is_latin_square(&a))
    ? printf("\nThis is a true latin square.\n")
    : printf("\nThis is NOT a true latin square.\n");

  free(a.array);


  
  return 0;
}
