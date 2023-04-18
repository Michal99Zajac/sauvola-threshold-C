#include "tools.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void sauvola_threshold(unsigned char **grayscale, unsigned char **output,
                       int num_cols, int num_rows, float k, int r, float R);

void sauvola_threshold_with_integral_image(unsigned char **grayscale,
                                           unsigned long long ***integral_image,
                                           unsigned char **output, int num_cols,
                                           int num_rows, float k, int r,
                                           float R);
