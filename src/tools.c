#include "pgm.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                                    Tools                                   */
/* -------------------------------------------------------------------------- */

/**
 * This function skips comments in a file.
 * It reads characters from the file until it reaches a non-whitespace
 * character, and then determines whether that character is the start of a
 * comment. If it is, it reads the entire comment line and recursively calls
 * itself to skip any additional comments.
 * If it is not the start of a comment, it seeks back one character so the next
 * read will start with the correct character.
 */
void skip_comments(FILE *file) {
  int ch;         // current character
  char line[256]; // buffer to store comment line

  // read characters until a non-whitespace character is found
  while ((ch = fgetc(file)) != EOF && isspace(ch))
    ;

  // if the non-whitespace character is the start of a comment
  if (ch == '#') {
    fgets(line, sizeof(line), file); // read the entire comment line

    // recursively call itself to skip any additional comments
    skip_comments(file);
  } else {
    fseek(file, -1, SEEK_CUR); // seek back one character
  }
}

/*
 * This function allocates a 2D array of arbitrary type and returns a pointer to
 * the array. The function takes the number of rows, columns and size of the
 * data type as input arguments. It returns a void** pointer to the array.
 */
unsigned char **alloc_2D_unsigned_char(int num_rows, int num_cols) {
  int i;

  // Allocate memory for the row pointers
  unsigned char **arr =
      (unsigned char **)malloc(num_rows * sizeof(unsigned char *));

  // Allocate memory for the data and assign it to the first row pointer
  arr[0] = (unsigned char *)malloc(num_rows * num_cols * sizeof(unsigned char));

  // Assign the remaining row pointers to point to the appropriate location in
  // the data
  for (i = 1; i < num_rows; i++) {
    arr[i] = arr[i - 1] + num_cols;
  }

  // Return the pointer to the 2D array
  return arr;
}

/**
 * Allocates memory for an integral image with two channels (sum and sum of
 * squares).
 *
 * @param num_rows The number of rows in the integral image.
 * @param num_cols The number of columns in the integral image.
 * @return A 3D array of unsigned long long representing the integral image.
 */
unsigned long long ***alloc_integral_image(int num_rows, int num_cols) {
  // Allocate memory for the first dimension
  unsigned long long ***integral_image =
      (unsigned long long ***)malloc(num_rows * sizeof(unsigned long long **));

  // Allocate memory for the second dimension
  integral_image[0] = (unsigned long long **)malloc(
      num_rows * num_cols * sizeof(unsigned long long *));

  // Allocate memory for the third dimension
  integral_image[0][0] = (unsigned long long *)malloc(
      num_rows * num_cols * 2 * sizeof(unsigned long long));

  // Set up the pointers for the second and third dimensions
  for (int i = 0; i < num_rows; i++) {
    if (i > 0) {
      integral_image[i] = integral_image[0] + i * num_cols;
      integral_image[i][0] = integral_image[0][0] + i * num_cols * 2;
    }
    for (int j = 1; j < num_cols; j++) {
      integral_image[i][j] = integral_image[i][j - 1] + 2;
    }
  }

  return integral_image;
}

/**
 * Computes the integral image of a given 2D array of integers.
 * An integral image, also known as a summed area table, is a data structure
 * used for quick and efficient calculation of the sum of values in a
 * rectangular subset of an image. The value at each pixel in the integral image
 * is the sum of all the pixels above and to the left of it in the original
 * image, inclusive. This function takes the input array, its dimensions, and an
 * output array and computes the integral image in place. The resulting integral
 * image is stored in the output array. The computation is done in
 * O(num_cols*num_cols) time, where num_cols and num_rows are the dimensions of
 * the input array.
 */
void compute_integral_image(unsigned char **input, unsigned long long ***output,
                            int num_cols, int num_rows) {
  int i, j;

  // Compute the first row of the integral image
  for (j = 0; j < num_cols; j++) {
    output[0][j][0] = input[0][j];
    output[0][j][1] = pow(input[0][j], 2);
    if (j > 0) {
      output[0][j][0] += output[0][j - 1][0];
      output[0][j][1] += output[0][j - 1][1];
    }
  }

  // Compute the first column of the integral image
  for (i = 1; i < num_rows; i++) {
    output[i][0][0] = input[i][0] + output[i - 1][0][0];
    output[i][0][1] = pow(input[i][0], 2) + output[i - 1][0][1];
  }

  // Compute the rest of the integral image
  for (i = 1; i < num_rows; i++) {
    for (j = 1; j < num_cols; j++) {
      output[i][j][0] = input[i][j] + output[i - 1][j][0] +
                        output[i][j - 1][0] - output[i - 1][j - 1][0];
      output[i][j][1] = pow(input[i][j], 2) + output[i - 1][j][1] +
                        output[i][j - 1][1] - output[i - 1][j - 1][1];
    }
  }
}
