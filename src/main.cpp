/* #include "DemoGame/DemoGame.h" */
#include "Pixels/Pixels.h"

int main() {
  /*
  DemoGame demo = DemoGame();
  demo.setup();
  demo.run();
  */

  PixelsGame game = PixelsGame();
  game.setup();
  game.run();
  return 0;
}
