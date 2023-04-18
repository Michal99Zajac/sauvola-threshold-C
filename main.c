#include "flow.h"
#include <stdio.h>

/* -------------------------------------------------------------------------- */
/*                                Main Program                                */
/* -------------------------------------------------------------------------- */

enum FLOW { PURE, INTEGRAL_IMAGE };

int main(int argc, char **argv) {
  double time;

  enum FLOW flow = PURE;

  switch (flow) {
  case INTEGRAL_IMAGE:
    time = pgm_sauvola_flow_with_integral_image(
        "./media/016_lanczos.pgm", "./media/016_lanczos_converted_ii.pgm");
    printf("Sauvola with Integral Image\n");
    break;
  case PURE:
    time = pgm_sauvola_flow("./media/016_lanczos.pgm",
                            "./media/016_lanczos_converted.pgm");
    printf("Sauvola\n");
    break;
  default:
    return 0;
  }

  printf("Time: %f\n", time);

  return 0;
}
