#include "pgm.h"
#include "sauvola.h"
#include "tools.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                                Program Flows                               */
/* -------------------------------------------------------------------------- */

double pgm_sauvola_flow(const char *input_file_name,
                        const char *output_file_name) {
  int num_rows, num_cols;
  int max_color;
  int header_length, i, j;
  clock_t start_time, end_time;
  double elapsed_time;

  // Read header to get dimensions and max color value
  if ((header_length = read_pgm_header(input_file_name, &num_rows, &num_cols,
                                       &max_color)) <= 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **grayscale = alloc_2D_unsigned_char(num_rows, num_cols);

  // read PGM image data
  if (read_pgm_data(grayscale[0], input_file_name, header_length, num_rows,
                    num_cols, max_color) == 0)
    ;

  // Allocate memory for output array
  unsigned char **output = alloc_2D_unsigned_char(num_rows, num_cols);

  // start timing
  start_time = clock();

  // Sauvola threshold
  sauvola_threshold(grayscale, output, num_cols, num_rows, 0.5, 13, 255);

  // end timing
  end_time = clock();

  // calculate elapsed time in milliseconds
  elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

  // write pgm file
  if (write_pgm_image(output_file_name, output[0], num_rows, num_cols, 255) ==
      0)
    exit(1);

  free(grayscale[0]);
  free(grayscale);
  free(output[0]);
  free(output);

  return elapsed_time;
}

double pgm_sauvola_flow_with_integral_image(const char *input_file_name,
                                            const char *output_file_name) {
  int num_rows, num_cols;
  int max_color;
  int header_length, i, j;
  clock_t start_time, end_time;
  double elapsed_time;

  // Read header to get dimensions and max color value
  if ((header_length = read_pgm_header(input_file_name, &num_rows, &num_cols,
                                       &max_color)) <= 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **grayscale = alloc_2D_unsigned_char(num_rows, num_cols);

  // read PGM image data
  if (read_pgm_data(grayscale[0], input_file_name, header_length, num_rows,
                    num_cols, max_color) == 0)
    ;

  // Allocate memory for output array
  unsigned char **output = alloc_2D_unsigned_char(num_rows, num_cols);

  // Allocate memory for integral image 3D array
  unsigned long long ***integral_image =
      alloc_integral_image(num_rows, num_cols);

  // start timing
  start_time = clock();

  // Calculate integral image
  compute_integral_image(grayscale, integral_image, num_cols, num_rows);

  // Sauvola threshold
  sauvola_threshold_with_integral_image(grayscale, integral_image, output,
                                        num_cols, num_rows, 0.5, 13, 255);

  // end timing
  end_time = clock();

  // calculate elapsed time in milliseconds
  elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

  // write pgm file
  if (write_pgm_image(output_file_name, output[0], num_rows, num_cols, 255) ==
      0)
    exit(1);

  free(grayscale[0]);
  free(grayscale);
  free(output[0]);
  free(output);
  free(integral_image[0][0]);
  free(integral_image[0]);
  free(integral_image);

  return elapsed_time;
}