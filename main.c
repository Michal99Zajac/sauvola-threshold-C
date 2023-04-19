#include "flow.h"
#include "tools.h"
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                                Main Program                                */
/* -------------------------------------------------------------------------- */

enum FLOW { PURE, INTEGRAL_IMAGE, TEST };

int main(int argc, char **argv) {
  double time;

  enum FLOW flow = INTEGRAL_IMAGE;

  switch (flow) {
  case INTEGRAL_IMAGE:
    time = pgm_sauvola_flow_with_integral_image(
        "./media/016_lanczos.pgm", "./media/016_lanczos_converted_ii.pgm", 10);
    printf("Sauvola with Integral Image\n");
    printf("Time: %f\n", time);
    break;
  case PURE:
    time = pgm_sauvola_flow("./media/016_lanczos.pgm",
                            "./media/016_lanczos_converted.pgm", 10);
    printf("Sauvola\n");
    printf("Time: %f\n", time);
    break;
  case TEST:
    if (test_integral_imgage("./media/016_lanczos.pgm")) {
      printf("TEST: pass\n");
    } else {
      printf("TEST: fail\n");
    }
  default:
    return 0;
  }

  return 0;
}
