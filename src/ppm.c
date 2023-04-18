#include "sauvola.h"
#include "tools.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                            PPM (Portable Pixmap)                           */
/* -------------------------------------------------------------------------- */

/**
 * This function reads the header of a PPM image file and returns the length of
 * the header in bytes. It takes as input the name of the file, and pointers to
 * integers to store the number of rows, columns, and the maximum color value.
 * If the file cannot be opened, or if it is not a valid PPM image file, the
 * function returns 0.
 */
int read_ppm_header(const char *filename, int *num_rows, int *num_cols,
                    int *max_color) {
  FILE *file;
  size_t file_lenght, header_length;
  char signature[3]; // PPM signature string "P6"

  // open file for reading in binary mode
  if ((file = fopen(filename, "rb")) == NULL) {
    return 0; // unable to open file
  }

  // get file length and return to beginning of file
  fseek(file, 0, SEEK_END);
  file_lenght = ftell(file);
  fseek(file, 0, SEEK_SET);

  // read the PPM signature and check that it is valid
  fgets(signature, sizeof(signature), file);
  if (signature[0] != 'P' || signature[1] != '6') {
    fclose(file);
    return 0; // not a valid PPM file
  }

  skip_comments(file); // skip over any comments in the header

  // read the number of columns, rows, and maximum color value from the header
  fscanf(file, "%d", num_cols);
  skip_comments(file);
  fscanf(file, "%d", num_rows);
  skip_comments(file);
  fscanf(file, "%d", max_color);

  // read the newline character at the end of the header
  fgetc(file);

  // calculate the header length and close the file
  header_length = ftell(file);
  fclose(file);

  // check that the data size in bytes matches the expected size
  if ((*num_rows) * 3 * (*num_cols) != (file_lenght - header_length)) {
    return 0; // incorrect data size
  }

  return header_length; // return header length
}

/**
 * This function writes a PPM image file with the given filename and image data
 * in RGB format. It takes as input the name of the file, pointers to the red,
 * green, and blue channel arrays, the number of rows and columns in the image,
 * and the maximum color value. If the file cannot be opened for writing, the
 * function returns 0. Otherwise, it returns 1.
 */
int write_ppm_data(const char *file_name, unsigned char *red_channel,
                   unsigned char *green_channel, unsigned char *blue_channel,
                   int num_rows, int num_cols, int max_color) {
  long i, total_pixels; // width times height
  FILE *file;

  // open file for writing in binary mode
  if ((file = fopen(file_name, "wb")) == NULL) {
    return 0; // unable to open file
  }

  // write the PPM header to the file
  fprintf(file, "P6\n%d %d\n# eyetom.com\n%d\n", num_cols, num_rows, max_color);

  total_pixels = num_rows * num_cols;
  for (i = 0; i < total_pixels; i++) {
    // Write the image data in pixel interleaved format (R, G, B)
    fputc(red_channel[i], file);
    fputc(green_channel[i], file);
    fputc(blue_channel[i], file);
  }

  // close the file and return success
  fclose(file);
  return 1;
}

/**
 * This function reads the pixel data from a PPM image file and stores it in the
 * RGB channel arrays. It takes as input pointers to the red, green, and blue
 * channel arrays, the filename of the image, the header length, the number of
 * rows and columns, and the maximum color value. It returns 1 if the read
 * operation was successful, and 0 otherwise.
 */
int read_ppm_data(unsigned char *red_channel, unsigned char *green_channel,
                  unsigned char *blue_channel, const char *filename,
                  int header_length, int num_rows, int num_cols,
                  int max_color) {
  long i, total_pixels;
  FILE *file;

  // Check if the image has only 1-byte color values
  if (max_color > 255) {
    return 0;
  }

  // Open the file for reading in binary mode
  if ((file = fopen(filename, "rb")) == NULL) {
    return 0;
  }

  // Seek to the start of the pixel data
  fseek(file, header_length, SEEK_SET);

  // Read the pixel data into the RGB channel arrays
  total_pixels = num_rows * num_cols;
  for (i = 0; i < total_pixels; i++) {
    red_channel[i] = (unsigned char)fgetc(file);
    green_channel[i] = (unsigned char)fgetc(file);
    blue_channel[i] = (unsigned char)fgetc(file);
  }

  // Close the file and return success
  fclose(file);
  return 1;
}

/**
 * This function writes a PPM image file with the given filename and image data
 * in RGB format. It takes as input the name of the file, pointers to the red,
 * green, and blue channel arrays, the number of rows and columns in the image,
 * and the maximum color value. If the file cannot be opened for writing, the
 * function returns 0. Otherwise, it returns 1.
 */
int write_ppm_image(const char *file_name, unsigned char *red_channel,
                    unsigned char *green_channel, unsigned char *blue_channel,
                    int num_rows, int num_cols, int max_color) {
  // Declare variables
  long total_pixels;
  int i;
  FILE *file;

  // Attempt to open the file in binary mode
  if ((file = fopen(file_name, "wb")) == NULL) {
    return 0;
  }

  // Write PPM header to file
  fprintf(file, "P6\n%d %d\n# eyetom.com\n%d\n", num_cols, num_rows, max_color);

  // Calculate total number of pixels in image
  total_pixels = num_rows * num_cols;

  // Write the image data in pixel interleaved format
  for (i = 0; i < total_pixels; i++) {
    fputc(red_channel[i], file);
    fputc(green_channel[i], file);
    fputc(blue_channel[i], file);
  }

  // Close the file
  fclose(file);

  // Return success
  return 1;
}

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