#include "tools.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* -------------------------------------------------------------------------- */
/*                       Sauvola Thresholding Algorithm                       */
/* -------------------------------------------------------------------------- */

/**
 * Implements the Sauvola thresholding algorithm for binarizing grayscale
 * images.
 *
 * @param grayscale A 2D array of unsigned char representing the input
 * grayscale image.
 * @param num_cols The number of columns in the input image.
 * @param num_rows The number of rows in the input image.
 * @param k The sensitivity parameter for the algorithm. Values between 0.2 and
 * 0.5 are typical.
 * @param r The window radius for the local region around each pixel. Values
 * between 10 and 20 are typical.
 * @param output A 2D array of unsigned char representing the output binary
 * image.
 * @param R The dynamic range of the image. Set to the maximum value of the
 * image data type.
 */
void sauvola_threshold(unsigned char **grayscale, unsigned char **output,
                       int num_cols, int num_rows, float k, int r, float R) {
  unsigned long long sum, sum_squares;
  long count;
  float mean, stdev, threshold;

  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      // Determine the bounds of the local region around the current pixel
      int left = fmax(j - r, 0);
      int right = fmin(j + r, num_cols - 1);
      int top = fmax(i - r, 0);
      int bottom = fmin(i + r, num_rows - 1);

      // Compute the mean and standard deviation of the pixel values in the
      // local region
      sum = 0;
      sum_squares = 0;
      for (int x = top; x <= bottom; x++) {
        for (int y = left; y <= right; y++) {
          sum += grayscale[x][y];
          sum_squares += pow(grayscale[x][y], 2);
        }
      }

      count =
          (bottom - top + 1) *
          (right - left +
           1); // can't be changed, the squar area could be different each loop
      mean = sum / count;
      stdev = sqrt((sum_squares / count) - (mean * mean));

      // Compute the threshold for the current pixel using the mean and standard
      // deviation
      threshold = mean * (1.0 + k * ((stdev / R) - 1.0));

      // Binarize the current pixel based on whether it is above or below the
      // threshold
      output[i][j] = grayscale[i][j] > threshold ? 255 : 0;
    }
  }
}

/**
 * Implements the Sauvola thresholding algorithm for binarizing grayscale
 * images using integral images for faster computation.
 *
 * @param grayscale A 2D array of unsigned char representing the input
 * grayscale image.
 * @param integral_image A 3D array representing the integral image.
 * @param num_cols The number of columns in the input image.
 * @param num_rows The number of rows in the input image.
 * @param k The sensitivity parameter for the algorithm. Values between 0.2 and
 * 0.5 are typical.
 * @param r The window radius for the local region around each pixel. Values
 * between 10 and 20 are typical.
 * @param output A 2D array of unsigned char representing the output binary
 * image.
 * @param R The dynamic range of the image. Set to the maximum value of the
 * image data type.
 */
void sauvola_threshold_with_integral_image(unsigned char **grayscale,
                                           unsigned long long ***integral_image,
                                           unsigned char **output, int num_cols,
                                           int num_rows, float k, int r,
                                           float R) {
  unsigned long long sum, sum_squares;
  long count;
  double mean, stdev, threshold;

  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      // Determine the bounds of the local region around the current pixel
      int left = fmax(j - r, 0);
      int right = fmin(j + r, num_cols - 1);
      int top = fmax(i - r, 0);
      int bottom = fmin(i + r, num_rows - 1);

      // Calculate integral image values for the local region
      unsigned long A = (top - 1 < 0 || left - 1 < 0)
                            ? 0
                            : integral_image[top - 1][left - 1][0];
      unsigned long B = top - 1 < 0 ? 0 : integral_image[top - 1][right][0];
      unsigned long C = left - 1 < 0 ? 0 : integral_image[bottom][left - 1][0];
      unsigned long D = integral_image[bottom][right][0];

      unsigned long A_sq = (top - 1 < 0 || left - 1 < 0)
                               ? 0
                               : integral_image[top - 1][left - 1][1];
      unsigned long B_sq = top - 1 < 0 ? 0 : integral_image[top - 1][right][1];
      unsigned long C_sq =
          left - 1 < 0 ? 0 : integral_image[bottom][left - 1][1];
      unsigned long D_sq = integral_image[bottom][right][1];

      // Compute the sum and sum of squares for the local region using integral
      // image values
      sum = D - B - C + A;
      sum_squares = D_sq - B_sq - C_sq + A_sq;

      // Compute the mean and standard deviation for the local region
      count =
          (right - left + 1) *
          (bottom - top +
           1); // can't be changed, the squar area could be different each loop
      mean = sum / (double)count;
      stdev = sqrt((sum_squares / (double)count) - (mean * mean));

      // Compute the threshold for the current pixel using the mean and standard
      // deviation
      threshold = mean * (1.0 + k * ((stdev / R) - 1.0));

      // Binarize the current pixel based on whether it is above or below the
      // threshold
      output[i][j] = grayscale[i][j] > threshold ? 255 : 0;
    }
  }
}