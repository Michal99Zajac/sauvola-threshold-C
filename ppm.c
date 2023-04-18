#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void ppm_sauvola_flow(const char *input_file_name,
                      const char *output_file_name) {
  int num_rows, num_cols;
  int max_color;
  int header_length, i, j;
  clock_t start_time, end_time;
  double elapsed_time;

  // Read header to get dimensions and max color value
  if ((header_length = read_ppm_header(input_file_name, &num_rows, &num_cols,
                                       &max_color)) <= 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **grayscale = alloc_2D_unsigned_char(num_rows, num_cols);

  // Allocate memory for RGB arrays
  unsigned char **red_channel = alloc_2D_unsigned_char(num_rows, num_cols);
  unsigned char **green_channel = alloc_2D_unsigned_char(num_rows, num_cols);
  unsigned char **blue_channel = alloc_2D_unsigned_char(num_rows, num_cols);

  // Read RGB data from file
  if (read_ppm_data(red_channel[0], green_channel[0], blue_channel[0],
                    input_file_name, header_length, num_rows, num_cols,
                    max_color) == 0)
    exit(1);

  // Compute grayscale values from RGB values
  unsigned char r, g, b, gray_value;
  for (i = 0; i < num_rows; ++i) {
    for (j = 0; j < num_cols; ++j) {
      r = red_channel[i][j];
      g = green_channel[i][j];
      b = blue_channel[i][j];
      gray_value = (unsigned char)((0.299 * r) + (0.587 * g) + (0.114 * b));
      grayscale[i][j] = gray_value;
    }
  }

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

  // print elapsed time
  printf("Sauvola Elapsed time: %f ms\n", elapsed_time);

  // write pgm file
  if (write_pgm_image(output_file_name, output[0], num_rows, num_cols, 255) ==
      0)
    exit(1);

  free(red_channel[0]);
  free(red_channel);
  free(green_channel[0]);
  free(green_channel);
  free(blue_channel[0]);
  free(blue_channel);
  free(grayscale[0]);
  free(grayscale);
  free(output[0]);
  free(output);
}

double ppm_sauvola_flow_with_integral_image(const char *input_file_name,
                                            const char *output_file_name) {
  int num_rows, num_cols;
  int max_color;
  int header_length, i, j;
  clock_t start_time, end_time;
  double elapsed_time;

  // Read header to get dimensions and max color value
  if ((header_length = read_ppm_header(input_file_name, &num_rows, &num_cols,
                                       &max_color)) <= 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **grayscale = alloc_2D_unsigned_char(num_rows, num_cols);

  // Allocate memory for RGB arrays
  unsigned char **red_channel = alloc_2D_unsigned_char(num_rows, num_cols);
  unsigned char **green_channel = alloc_2D_unsigned_char(num_rows, num_cols);
  unsigned char **blue_channel = alloc_2D_unsigned_char(num_rows, num_cols);

  // Read RGB data from file
  if (read_ppm_data(red_channel[0], green_channel[0], blue_channel[0],
                    input_file_name, header_length, num_rows, num_cols,
                    max_color) == 0)
    exit(1);

  // Compute grayscale values from RGB values
  unsigned char r, g, b, gray_value;
  for (i = 0; i < num_rows; ++i) {
    for (j = 0; j < num_cols; ++j) {
      r = red_channel[i][j];
      g = green_channel[i][j];
      b = blue_channel[i][j];
      gray_value = (unsigned char)((0.299 * r) + (0.587 * g) + (0.114 * b));
      grayscale[i][j] = gray_value;
    }
  }

  // Allocate memory for output array
  unsigned char **output = alloc_2D_unsigned_char(num_rows, num_cols);

  // Allocate memory for integral image 3D array
  unsigned long long ***integral_image =
      alloc_integral_image(num_rows, num_cols);

  // Calculate integral image
  compute_integral_image(grayscale, integral_image, num_cols, num_rows);

  // start timing
  start_time = clock();

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

  free(red_channel[0]);
  free(red_channel);
  free(green_channel[0]);
  free(green_channel);
  free(blue_channel[0]);
  free(blue_channel);
  free(grayscale[0]);
  free(grayscale);
  free(output[0]);
  free(output);
  free(integral_image[0][0]);
  free(integral_image[0]);
  free(integral_image);
}