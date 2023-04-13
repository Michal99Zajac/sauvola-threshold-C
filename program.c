#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/*                                    TOOLS                                   */
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
void **alloc_2D_array(int num_rows, int num_cols, size_t type_size) {
  int i;

  // Allocate memory for the row pointers
  void **arr = (void **)malloc(num_rows * sizeof(void *));

  // Allocate memory for the data and assign it to the first row pointer
  arr[0] = calloc(num_rows * num_cols, type_size);

  // Assign the remaining row pointers to point to the appropriate location in
  // the data
  for (i = 1; i < num_rows; i++) {
    arr[i] = arr[i - 1] + num_cols;
  }

  // Return the pointer to the 2D array
  return arr;
}

/**
 * This function performs simple binarization on a grayscale image map,
 * converting each pixel to either black or white based on a threshold value.
 */
void simple_binarization(unsigned char **grayscale_map, int threshold,
                         int num_rows, int num_cols) {
  int i, j;

  // Loop through each pixel in the image map.
  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      // If the pixel value is greater than the threshold, set it to white
      // (255). Otherwise, set it to black (0).
      grayscale_map[i][j] = grayscale_map[i][j] > threshold ? 255 : 0;
    }
  }
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
void compute_integral_image(unsigned char **input, long int **output,
                            int num_cols, int num_rows) {
  int i, j;

  // Compute the first row of the integral image
  for (j = 0; j < num_cols; j++) {
    output[0][j] = input[0][j];
    if (j > 0) {
      output[0][j] += output[0][j - 1];
    }
  }

  // Compute the first column of the integral image
  for (i = 1; i < num_rows; i++) {
    output[i][0] = input[i][0] + output[i - 1][0];
  }

  // Compute the rest of the integral image
  for (i = 1; i < num_rows; i++) {
    for (j = 1; j < num_cols; j++) {
      output[i][j] = input[i][j] + output[i - 1][j] + output[i][j - 1] -
                     output[i - 1][j - 1];
    }
  }
}

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

/* -------------------------------------------------------------------------- */
/*                                Main Program                                */
/* -------------------------------------------------------------------------- */

int main(int argc, char **argv) {
  int num_rows, num_cols;
  int max_color;
  int header_pos, i, j;

  char file_name[] = "./media/maly.ppm";
  char output_file_name[] = "./media/maly.pgm";

  // Read header to get dimensions and max color value
  if ((header_pos =
           read_ppm_header(file_name, &num_rows, &num_cols, &max_color)) <= 0)
    exit(1);

  // Allocate memory for RGB arrays
  unsigned char **red_channel = (unsigned char **)alloc_2D_array(
      num_rows, num_cols, sizeof(unsigned char));
  unsigned char **green_channel = (unsigned char **)alloc_2D_array(
      num_rows, num_cols, sizeof(unsigned char));
  unsigned char **blue_channel = (unsigned char **)alloc_2D_array(
      num_rows, num_cols, sizeof(unsigned char));

  // Read RGB data from file
  if (read_ppm_data(red_channel[0], green_channel[0], blue_channel[0],
                    file_name, header_pos, num_rows, num_cols, max_color) == 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **gray_channel = (unsigned char **)alloc_2D_array(
      num_rows, num_cols, sizeof(unsigned char));

  // Compute grayscale values from RGB values
  unsigned char r, g, b, gray_value;
  for (i = 0; i < num_rows; ++i) {
    for (j = 0; j < num_cols; ++j) {
      r = red_channel[i][j];
      g = green_channel[i][j];
      b = blue_channel[i][j];
      gray_value = (unsigned char)((0.299 * r) + (0.587 * g) + (0.114 * b));
      gray_channel[i][j] = gray_value;
    }
  }

  // Calculate integral image
  long int **integral_image =
      (long int **)alloc_2D_array(num_rows, num_cols, sizeof(long int));

  compute_integral_image(gray_channel, integral_image, num_cols, num_rows);

  // simple binarization with a fixed threshold
  // simple_binarization(gray_channel, 100, num_rows, num_cols);

  // write pgm file
  if (write_pgm_image(output_file_name, gray_channel[0], num_rows, num_cols,
                      255) == 0)
    exit(1);

  // Free memory
  free(red_channel[0]);
  free(red_channel);
  free(green_channel[0]);
  free(green_channel);
  free(blue_channel[0]);
  free(blue_channel);
  free(gray_channel[0]);
  free(gray_channel);
  free(integral_image[0]);
  free(integral_image);

  return 0;
}
