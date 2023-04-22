#include "flow.h"
#include "test.h"
#include "tools.h"
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                                Main Program                                */
/* -------------------------------------------------------------------------- */

enum FLOW { PURE, INTEGRAL_IMAGE, TEST_INTEGRAL_IMAGE, TEST_IMAGE_UNITY };

int main(int argc, char **argv) {
  double time;

  enum FLOW flow = TEST_INTEGRAL_IMAGE;

  switch (flow) {
  case INTEGRAL_IMAGE:
    time = pgm_sauvola_flow_with_integral_image(
        "./media/016_lanczos.pgm", "./media/016_lanczos_converted_ii.pgm", 13);
    printf("Sauvola with Integral Image\n");
    printf("Time: %f\n", time);
    break;
  case PURE:
    time = pgm_sauvola_flow("./media/016_lanczos.pgm",
                            "./media/016_lanczos_converted.pgm", 13);
    printf("Sauvola\n");
    printf("Time: %f\n", time);
    break;
  case TEST_INTEGRAL_IMAGE:
    if (test_integral_image("./media/016_lanczos.pgm")) {
      printf("TEST INTEGRAL IMAGE: pass\n");
    } else {
      printf("TEST INTEGRAL IMAGE: fail\n");
    }
    break;
  case TEST_IMAGE_UNITY:
    if (test_image_unity("./media/016_lanczos_converted_ii.pgm",
                         "./media/016_lanczos_converted.pgm")) {
      printf("TEST IMAGE UNITY: pass\n");
    } else {
      printf("TEST IMAGE UNITY: fail\n");
    }
    break;
  default:
    return 0;
  }

  return 0;
}
