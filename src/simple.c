/**
 * This function performs simple binarization on a grayscale image map,
 * converting each pixel to either black or white based on a threshold value.
 */
void simple_binarization(unsigned char **grayscale, unsigned char **output,
                         int threshold, int num_rows, int num_cols) {
  int i, j;

  // Loop through each pixel in the image map.
  for (i = 0; i < num_rows; i++) {
    for (j = 0; j < num_cols; j++) {
      // If the pixel value is greater than the threshold, set it to white
      // (255). Otherwise, set it to black (0).
      output[i][j] = grayscale[i][j] > threshold ? 255 : 0;
    }
  }
}
