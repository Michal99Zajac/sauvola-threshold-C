#include "pgm.h"
#include "tools.h"
#include <stdbool.h>

/* -------------------------------------------------------------------------- */
/*                              Application Tests                             */
/* -------------------------------------------------------------------------- */

/**
 * This function is used to test whether an image is correctly converted into an
 * integral image. An integral image is a representation of an image where each
 * pixel contains the sum of all the pixels above and to the left of it. The
 * function takes the path to the source image as a parameter, reads the header
 * to get the dimensions and max color value, allocates memory for the grayscale
 * and integral image arrays, reads the image data, computes the integral image,
 * and then loops through the pixels of the image, calculating the integral
 * image values for the local region and comparing them with the grayscale value
 * of the same pixel. If any of the values don't match, the function returns
 * false, indicating that the test has failed. If all the values match, the
 * function returns true, indicating that the test has passed.
 */
bool test_integral_image(const char *source_image) {
  int num_rows, num_cols;
  int max_color;
  int header_length, i, j;
  unsigned long long sum;

  // Read header to get dimensions and max color value
  if ((header_length = read_pgm_header(source_image, &num_rows, &num_cols,
                                       &max_color)) <= 0)
    exit(1);

  // Allocate memory for grayscale array
  unsigned char **grayscale = alloc_2D_unsigned_char(num_rows, num_cols);

  // read PGM image data
  if (read_pgm_data(grayscale[0], source_image, header_length, num_rows,
                    num_cols, max_color) == 0)
    ;

  // Allocate memory for integral image 3D array
  unsigned long long ***integral_image =
      alloc_integral_image(num_rows, num_cols);

  // Calculate integral image
  compute_integral_image(grayscale, integral_image, num_cols, num_rows);

  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      // Calculate integral image values for the local region
      unsigned long A =
          (i - 1 < 0 || j - 1 < 0) ? 0 : integral_image[i - 1][j - 1][0];
      unsigned long B = i - 1 < 0 ? 0 : integral_image[i - 1][j][0];
      unsigned long C = j - 1 < 0 ? 0 : integral_image[i][j - 1][0];
      unsigned long D = integral_image[i][j][0];

      sum = D - B - C + A;

      if (sum != grayscale[i][j])
        return false;
    }
  }

  return true;
}

/**
 * This function is used to test whether two given images are identical.
 * It takes the paths to the two source images as parameters, reads their
 * headers to obtain the dimensions and max color values, and checks if these
 * values match. If they do not match, the function returns false, indicating
 * that the images are not identical. If the dimensions and max color values
 * match, the function allocates memory for the grayscale arrays, reads the
 * image data, and then loops through the pixels of both images, comparing the
 * grayscale value of each corresponding pixel. If any of the grayscale values
 * don't match, the function returns false, indicating that the images are not
 * identical. If all the grayscale values match, the function returns true,
 * indicating that the images are identical.
 */
bool test_image_unity(const char *image_one, const char *image_two) {
  int num_rows_one, num_cols_one, num_rows_two, num_cols_two;
  int max_color_one, max_color_two;
  int header_length_one, header_length_two, i, j;

  // Read headers to get dimensions and max color values
  if ((header_length_one = read_pgm_header(image_one, &num_rows_one,
                                           &num_cols_one, &max_color_one)) <= 0)
    exit(1);

  if ((header_length_two = read_pgm_header(image_two, &num_rows_two,
                                           &num_cols_two, &max_color_two)) <= 0)
    exit(1);

  // Check if dimensions and max color values match
  if (num_rows_one != num_rows_one || num_cols_one != num_cols_two ||
      max_color_one != max_color_two)
    return false;

  // Allocate memory for grayscale arrays
  unsigned char **grayscale_image_one =
      alloc_2D_unsigned_char(num_rows_one, num_cols_one);
  unsigned char **grayscale_image_two =
      alloc_2D_unsigned_char(num_rows_two, num_cols_two);

  // Read PGM image data
  if (read_pgm_data(grayscale_image_one[0], image_one, header_length_one,
                    num_rows_one, num_rows_one, max_color_one) == 0)
    ;
  if (read_pgm_data(grayscale_image_two[0], image_two, header_length_two,
                    num_rows_two, num_rows_two, max_color_two) == 0)
    ;

  // Loop through pixels and compare grayscale values
  for (i = 0; i < num_rows_one; i++) {
    for (j = 0; j < num_cols_one; j++) {
      if (grayscale_image_one[i][j] != grayscale_image_two[i][j]) {
        return false;
      }
    }
  }

  // If all grayscale values match, return true
  return true;
}
