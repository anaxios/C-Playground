#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <gcrypt.h>
#include <ctype.h>

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

String_View sv_from_cstr(const char *cstr)
{
  return (String_View) {
    .count = strlen(cstr),
			  .data = cstr,
			  };
}

char *shift(int *argc, char ***argv)
{
  assert(*argc > 0);
  char *result = **argv;
  *argv += 1;
  *argc -= 1;
  return result;
}

#define GRID(n,r,c) (n)->array[(r) * 26 + (c)]

#define SV_NULL (String_View) {0}

#define PRINT_NL printf("\n");

#define PRINT_CELL(cell)			\
  printf("| %lu,%lu,%c |",			\
	 cell.row,				\
	 cell.col,				\
	 cell.elem);				\
  printf("\n");

#define PRINT_RC(rc)				\
  printf("| ");					\
  for (size_t i = 0; i < rc.length; i++) {	\
    printf("%lu,%lu,%c | ",			\
	   rc.array[i].row,			\
	   rc.array[i].col,			\
	   rc.array[i].elem);}			\
   printf("\n");

#define PRINT_PASS(rc)				\
  for (size_t i = 0; i < rc.length; i++) {	\
    printf("%c",rc.array[i].elem);}		\
  printf("\n");					\

typedef struct {
  size_t count;
  const unsigned char *data;
} Hash_View;

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

typedef struct {
  size_t length;
  Cell *array;
} Row_Col;

size_t exponent(size_t n)
{
  size_t temp = 1;
  for (size_t i = 0; i < n; i++) {
    temp += n * n;
  }
  return temp;
}

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

void swap_rows(Latin_Square *square, size_t row1, size_t row2)
{
  Cell *temp_row1 = (Cell*)malloc(square->key.count * sizeof(Cell));
  assert(temp_row1 != NULL);
  Cell *temp_row2 = (Cell*)malloc(square->key.count * sizeof(Cell));
  assert(temp_row2 != NULL);

  // Load temp variables
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].row == row1)
      temp_row1[tuple % square->key.count] = square->array[tuple];
    if (square->array[tuple].row == row2)
      temp_row2[tuple % square->key.count] = square->array[tuple];
  }
  // do the swap
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].row == row1) {
      square->array[tuple] = temp_row2[tuple % square->key.count];
      square->array[tuple].row = row1;
    } else if (square->array[tuple].row == row2) {
      square->array[tuple] = temp_row1[tuple % square->key.count];
      square->array[tuple].row = row2;
    }
  }

  free(temp_row1);
  free(temp_row2);
}

void swap_cols(Latin_Square *square, size_t col1, size_t col2)
{
  Cell *temp_col1 = (Cell*)malloc(square->key.count * sizeof(Cell));
  assert(temp_col1 != NULL);
  Cell *temp_col2 = (Cell*)malloc(square->key.count * sizeof(Cell));
  assert(temp_col2 != NULL);
  size_t temp_1 = 0;
  size_t temp_2 = 0;

  // load temp variables
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].col == col1) 
      temp_col1[temp_1++] = square->array[tuple];
    if (square->array[tuple].col == col2)
      temp_col2[temp_2++] = square->array[tuple];
  }

  // reset temp counters
  temp_1 = 0; temp_2 = 0;

  // make the swap
  for (size_t tuple = 0; tuple < square->size; tuple++) {
    if (square->array[tuple].col == col1) {
      square->array[tuple] = temp_col2[temp_2++];
      square->array[tuple].col = col1;
    } else if (square->array[tuple].col == col2) {
      square->array[tuple] = temp_col1[temp_1++];
      square->array[tuple].col = col2;
    }
  }

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
}

unsigned char *hash(Hash_View *key, unsigned char *buffer)
{
  unsigned int l = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  gcry_md_hd_t h;
  gcry_md_open(&h, GCRY_MD_SHA512, GCRY_MD_FLAG_SECURE);
  //gcry_md_enable(h, GCRY_MD_SHA512); // use for multiple hash functions
  gcry_md_write(h, key->data, key->count);
  unsigned char *x = gcry_md_read(h, GCRY_MD_SHA512);

  for (size_t i = 0; i < l; i++) {
    buffer[i] = x[i];
  }

  gcry_md_close(h);
  return buffer;
}

#define HASH_ROUNDS 1

void encode_square(Latin_Square *square, const String_View *key)
{
  unsigned int l = gcry_md_get_algo_dlen(GCRY_MD_SHA512);
  unsigned char *buffer = (unsigned char*)malloc(l);
  assert(buffer != NULL);
  Hash_View x;
  x.data = (unsigned char*)key->data;
  x.count = key->count;
  x.data = hash(&x, buffer);
  x.count = sizeof(x.data);
  
  size_t n = 0;
  // set n < a larger number to encode with a hash of the previous hash
  while (n < HASH_ROUNDS ) {
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
	swap_cols(square,
		  x.data[index] % square->key.count,
		  x.data[index + 1] % square->key.count);
      }
    }

    x.data = hash(&x, buffer);
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
  size_t r = -1; // so first iteration row will be set to 0

  for (size_t tuple = 0; tuple < square.size; tuple++) {
    if (tuple % key.count == 0) r++;
    square.array[tuple] = (Cell) { .row = r,
			       .col = (tuple % key.count),
			       .elem = key.data[(r + tuple) % key.count]};
  }

  return square;
}

Row_Col get_column(Latin_Square *square, size_t col)
{
  Row_Col temp;
  temp.array = (Cell*)malloc(square->key.count * sizeof(Cell));
  temp.length = square->key.count;
  assert(temp.array != NULL);
  size_t temp_index = 0;
  
  for (size_t sq_index = 0; sq_index < square->size; sq_index++) {
    if (square->array[sq_index].col == col) {
      temp.array[temp_index] = square->array[sq_index];
      /* printf("%lu,%c,%lu ", */
      /* 	     temp.array[temp_index].row, */
      /* 	     temp.array[temp_index].elem, */
      /* 	     temp.array[temp_index].col); */
      temp_index++;
    }
  }
  
  return temp;
}

Row_Col get_row(Latin_Square *square, size_t row)
{
  Row_Col temp;
  temp.array = (Cell*)malloc(square->key.count * sizeof(Cell));
  temp.length = square->key.count;
  assert(temp.array != NULL);
  size_t temp_index = 0;

  // search through square
  for (size_t sq_index = 0; sq_index < square->size; sq_index++) {
    if (square->array[sq_index].row == row) {
      temp.array[temp_index] = square->array[sq_index];
      /* printf("%lu,%c,%lu ", */
      /* 	     temp.array[temp_index].row, */
      /* 	     temp.array[temp_index].elem, */
      /* 	     temp.array[temp_index].col); */
      temp_index++;
    }
  }
  
  return temp;
}

Cell find_start_point(Latin_Square *square, String_View url)
{
  // Alternating rows and columns find the first 6 letters
  // and then the first letter again as the 7th element
  // then return that Cell
  Cell result;
  //result.array = (Cell*)malloc(url.count * 2 * sizeof(Cell));
  //result.length = url.count * 2;
  // PRINT_RC(result);
  // ssize_t result_index = 0;
  size_t url_index = 0;
  ssize_t double_mark = 0;  
  // get first elements
  Row_Col col;
  Row_Col row = get_row(square, 0);
  //   PRINT_RC(row);
  for (size_t r_index = 0; r_index < row.length; r_index++) {
    if (row.array[r_index].elem == url.data[url_index]) {	  
      //      if (start_point.col < row.array[r_index].col) {
      //	result.array[result_index] = row.array[(1 + r_index) % row.length];
      //		result_index++;
      result = row.array[r_index];
      if (row.array[r_index].elem == url.data[url_index + 1]) double_mark++;
		//} else { // if (result.array[result_index - 1].col > row.array[r_index].col) {
		//result.array[result_index] = row.array[(r_index - 1) % row.length];
		//	result_index++;
		//	result.array[result_index] = row.array[(r_index - 2) % row.length];
    }
  }
  
  url_index++;

  // get next elements based off the first
  for (size_t index = 0; index < url.count; index++) {
    if (index % 2 == 0) {
      col = get_column(square, (result.col + double_mark) % row.length);
      double_mark = 0;
      //PRINT_RC(col);
      for (size_t c_index = 0; c_index < col.length; c_index++) {
	if (col.array[c_index].elem == url.data[url_index]) {
	  // PRINT_CELL(result);
	  if (result.row < col.array[c_index].row) {
	    //	    result_index++;
	    result = col.array[c_index];
	    //	    result_index++;
	    //	    result.array[result_index] = col.array[(c_index + 2) % col.length];
	    if (col.array[c_index].elem == url.data[url_index + 1]) double_mark++;
	  } else {
		  //	    result_index++;
	    result = col.array[c_index];
	    //	    result_index++;
	    //	    result.array[result_index] = col.array[(c_index - 2) % col.length];
	    if (col.array[c_index].elem == url.data[url_index + 1]) double_mark--;
	  }
	}
      }
      url_index++;      
    } else {
      row = get_row(square, (result.row + double_mark) % row.length);
      double_mark = 0;
      //PRINT_RC(row);
      for (size_t r_index = 0; r_index < row.length; r_index++) {
	if (row.array[r_index].elem == url.data[url_index]) {	  
	  //PRINT_RC(result);
	  if (result.col < row.array[r_index].col) {
	    //	    result_index++;
	    result = row.array[r_index];
	    // 	    result_index++;
	  //	    result.array[result_index] = row.array[(r_index + 2) % row.length];
	    if (row.array[r_index].elem == url.data[url_index + 1]) double_mark++;
	  } else { //if (result.array[result_index].col > row.array[r_index].col) {
	  //	    result_index++;
	    result = row.array[r_index];
	    //	     result_index++;
	    //	    result.array[result_index] = row.array[(r_index - 2) % row.length];
	    if (row.array[r_index].elem == url.data[url_index + 1]) double_mark--;	    
	  }
	}
      }
      url_index++;
    }
  }
    
  return result;
}

size_t mod(ssize_t a, size_t b) {return ((a / b * b) + a) % b;}

Row_Col encode_password(Latin_Square *square, String_View url, Cell start_point)
{
  // Alternating rows and columns find the first 6 letters
  // and then the first letter again as the 7th element
  // then return that Cell
  
  Row_Col result;
  result.array = (Cell*)malloc(url.count * 2 * sizeof(Cell));
  assert(result.array != NULL);
  result.length = url.count * 2;
  printf("Starting Point: ");
  PRINT_CELL(start_point);
  size_t result_index = 0;
  size_t url_index = 0;
  ssize_t double_mark = 0;

  // get first elements
  Row_Col row = get_row(square, start_point.row);
  for (size_t r_index = 0; r_index < row.length; r_index++) {
    if (row.array[r_index].elem == url.data[url_index]) {	  
      if (start_point.col < row.array[r_index].col) {
	result.array[result_index] = row.array[mod((1 + r_index), row.length)];
	result_index++;
	result.array[result_index] = row.array[mod((2 + r_index), row.length)];
	if (row.array[result_index].elem == url.data[url_index + 1]) double_mark++;
      } else {
	result.array[result_index] = row.array[mod((r_index - 1), row.length)];
	result_index++;
	result.array[result_index] = row.array[mod((r_index - 2), row.length)];
	if (row.array[result_index].elem == url.data[url_index + 1]) double_mark--;
      }
    }
  }
  url_index++;

  // get next elements based of the first
  for (size_t index = 0; index < url.count; index++) {
    if (index % 2 == 0) {
      Row_Col col = get_column(square, mod((result.array[result_index].col + double_mark), row.length));
      double_mark = 0;
      for (size_t c_index = 0; c_index < col.length; c_index++) {
	if (col.array[c_index].elem == url.data[url_index]) {
	  if (result.array[result_index].row < col.array[c_index].row) {
	    result_index++;
	    result.array[result_index] = col.array[mod((c_index + 1), col.length)];
	    result_index++;
	    result.array[result_index] = col.array[mod((c_index + 2), col.length)];
	    if (col.array[result_index].elem == url.data[url_index + 1]) double_mark++;
	  } else { 
	    result_index++;
	    result.array[result_index] = col.array[mod((c_index - 1), col.length)];
	    result_index++;
	    result.array[result_index] = col.array[mod((c_index - 2), col.length)];
	    if (col.array[result_index].elem == url.data[url_index + 1]) double_mark--;
	  }
	}
      }
      
      url_index++;      
    } else {
      row = get_row(square, mod((result.array[result_index].row + double_mark), row.length));
      double_mark = 0;
      for (size_t r_index = 0; r_index < row.length; r_index++) {
	if (row.array[r_index].elem == url.data[url_index]) {	  
	  if (result.array[result_index].col < row.array[r_index].col) {
	    result_index++;
	    result.array[result_index] = row.array[mod((r_index + 1), row.length)];
	    result_index++;
	    result.array[result_index] = row.array[mod((r_index + 2), row.length)];
	    if (row.array[result_index].elem == url.data[url_index + 1]) double_mark++;
	  } else {
	    result_index++;
	    result.array[result_index] = row.array[mod((r_index - 1), row.length)];
	    result_index++;
	    result.array[result_index] = row.array[mod((r_index - 2), row.length)];
	    if (row.array[result_index].elem == url.data[url_index + 1]) double_mark--;	    
	  }
	}
      }
      
      url_index++;
    }
  }
      
  return result;
}

void print_latin_square(Latin_Square *square, size_t flag)
{
  // set flag to 0 for elems only or 1 to set row, col and elem
  if (flag == 0) {
    printf("0 1 2 3 4 5 6 7 8 9 - . \n");
    for (size_t tuple = 0; tuple < square->size; tuple++)  {
      printf("%c ", square->array[tuple].elem);
    if (square->array[tuple].col == (square->key.count - 1)) printf("\n");
    }
  } else {
    for (size_t tuple = 0; tuple < square->size; tuple++)  {
      printf("|%lu,%lu,%c|",
	     square->array[tuple].row,
	     square->array[tuple].col,
	     square->array[tuple].elem);
      if (square->array[tuple].col == (square->key.count - 1)) printf("\n"); 
    }
  }
}

#define URL_LENGTH_TO_USE 6
String_View sanitize_string(Latin_Square *square, String_View url)
{
  char *result = (char*)malloc(URL_LENGTH_TO_USE * sizeof(char));
  assert(result != NULL);
  Row_Col first_row = get_row(square, 0);

  if (url.count < URL_LENGTH_TO_USE) {
    printf("url too short\n");
    exit(1);
  }
  
  for (size_t index = 0; index < URL_LENGTH_TO_USE; index++) {
    // whitelist only valid URL chars
    if (!((url.data[index] > 64 && url.data[index] < 91)
	  || (url.data[index] > 96 && url.data[index] < 123)
	  || (url.data[index] > 44 && url.data[index] < 47)
	  || (url.data[index] > 47 && url.data[index] < 58))) {
      printf("URL contains invalid character: %c\n", url.data[index]);
      exit(1);    
    } else {
      // account for digits 
      if (url.data[index] > 47 && url.data[index] < 58) {
	size_t i = url.data[index] - 48;
	result[index] = first_row.array[i].elem;
      } else if (url.data[index] == 45) { // '-'
	result[index] = first_row.array[10].elem;
      } else if (url.data[index] == 46) { // '.'
	result[index] = first_row.array[11].elem;
      } else {
	result[index] = toupper(url.data[index]);
      }
    }
  }
  PRINT_NL;
  printf("Sanitized url: ");
  printf("%s\n", result);
  return (String_View) { .data = result, .count = URL_LENGTH_TO_USE };
}

int main(int argc, char *argv[])
{
  const char *const program = shift(&argc, &argv);        // skip program
  
  String_View seed = SV("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  Latin_Square a = make_latin_square(seed);
  
  if (argc == 0) {
    PRINT_NL;
    printf("Here's a random latin square:\n");
    randomize_square(&a);
    print_latin_square(&a, 0);
    PRINT_NL;
    printf("Help message goes here\n");
    PRINT_NL;
    exit(0);
  } else if (argc == 1) {
    const String_View key = sv_from_cstr(shift(&argc, &argv));
    encode_square(&a, &key);
    PRINT_NL;
    print_latin_square(&a, 0);
    PRINT_NL;    
  } else if (argc == 2) {
    const String_View key = sv_from_cstr(shift(&argc, &argv));
    encode_square(&a, &key);
    const String_View url = sanitize_string(&a, sv_from_cstr(shift(&argc, &argv)));
    Cell start = find_start_point(&a, url);
    Row_Col pass = encode_password(&a, url, start);
    PRINT_NL;
    print_latin_square(&a, 0);
    PRINT_NL;
    //PRINT_RC(pass);
    printf("Password: ");
    PRINT_PASS(pass);
    PRINT_NL;
  } else {printf("%s\n exited very badly", program); exit(1);}

  //(is_latin_square(&a))
  //  ? printf("\nThis is a true latin square.\n")
  //  : printf("\nThis is NOT a true latin square.\n");
  //get_column(&a, 6);
  free(a.array);
  
  
  // TODO : make better errors
  // TODO : figure out upper lower case scheme

  return 0;
}
