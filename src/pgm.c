#include "sauvola.h"
#include "tools.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                           PGM (Portable Gray Map)                          */
/* -------------------------------------------------------------------------- */

/*
 * Read header of a PGM binary file and extract image dimensions and max color
 * value. Return the length of the header in bytes if the file is valid,
 * otherwise return 0.
 */
int read_pgm_header(const char *file_name, int *num_rows, int *num_cols,
                    int *max_color) {
  FILE *file_pointer;
  size_t file_length, header_length;
  char signature[3]; // PGM signature string "P5"

  // Try to open the file for binary reading
  if ((file_pointer = fopen(file_name, "rb")) == NULL) {
    return 0;
  }

  // Determine the length of the file and set file pointer to the beginning
  fseek(file_pointer, 0, SEEK_END);
  file_length = ftell(file_pointer);
  fseek(file_pointer, 0, SEEK_SET);

  // Read the signature and check if it's a valid PGM binary file
  fgets(signature, sizeof(signature), file_pointer);
  if (signature[0] != 'P' || signature[1] != '5') {
    fclose(file_pointer);
    return 0;
  }

  // Skip any comments in the header and read the dimensions and max color value
  skip_comments(file_pointer);
  fscanf(file_pointer, "%d", num_cols);
  skip_comments(file_pointer);
  fscanf(file_pointer, "%d", num_rows);
  skip_comments(file_pointer);
  fscanf(file_pointer, "%d", max_color);
  fgetc(file_pointer);

  // Determine the length of the header and close the file
  header_length = ftell(file_pointer);
  fclose(file_pointer);

  // If the file size does not match the expected image size, it's not a valid
  // PGM file
  if ((*num_rows) * (*num_cols) != (file_length - header_length)) {
    return 0;
  }

  return header_length;
}

/**
 * This function reads in pixel data from a Portable Graymap (PGM) image file
 * with the given filename and populates an unsigned char buffer with the pixel
 * data. It takes as input a pointer to the image buffer, the name of the file
 * to read from, the length of the header in bytes, the number of rows and
 * columns in the image, and the maximum pixel value. If the file cannot be
 * opened for reading, the function returns 0. If the number of rows read does
 * not match the expected number, the function also returns 0. Otherwise, the
 * function returns 1 to indicate success.
 */
int read_pgm_data(unsigned char *image, const char *file_name,
                  int header_length, int num_rows, int num_cols, int max_val) {
  FILE *file;

  // Attempt to open the file in binary mode
  if ((file = fopen(file_name, "rb")) == NULL) {
    return 0;
  }

  // Seek past the header to the pixel data
  fseek(file, header_length, SEEK_SET);

  // Read the pixel data into the buffer
  int rows_read = fread(image, num_cols, num_rows, file);

  // Close the file
  fclose(file);

  // Check if the number of rows read matches the expected number
  if (num_rows != rows_read) {
    return 0;
  }

  // Return success
  return 1;
}

/**
 * This function writes the given image data to a PGM image file with the
 * specified filename. It takes as input the name of the file, a pointer to
 * the image data, the number of rows and columns in the image, and the maximum
 * color value. If the file cannot be opened for writing, the function returns
 * 0. Otherwise, it returns 1.
 */
int write_pgm_image(const char *file_name, unsigned char *image_data,
                    int num_rows, int num_cols, int max_val) {
  FILE *file;

  // Attempt to open the file for writing in binary mode
  if ((file = fopen(file_name, "wb")) == NULL) {
    return 0;
  }

  // Write the header information to the file
  fprintf(file, "P5\n%d %d\n# eyetom.com\n%d\n", num_cols, num_rows, max_val);

  // Write the image data to the file
  int rows_written = fwrite(image_data, num_cols, num_rows, file);

  // Close the file
  fclose(file);

  // Check if the number of rows written matches the expected number
  if (num_rows != rows_written) {
    return 0;
  }

  // Return success
  return 1;
}
