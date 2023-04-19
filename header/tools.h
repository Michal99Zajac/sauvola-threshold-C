#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void skip_comments(FILE *file);

unsigned char **alloc_2D_unsigned_char(int num_rows, int num_cols);

unsigned long long ***alloc_integral_image(int num_rows, int num_cols);

void compute_integral_image(unsigned char **input, unsigned long long ***output,
                            int num_cols, int num_rows);

bool test_integral_imgage(const char *source_image);
