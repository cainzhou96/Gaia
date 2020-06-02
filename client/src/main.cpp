#include "main.h"

#ifndef __APPLE__
#include <SDL.h>
#undef main
#endif


#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(void) {
  // Create the client.
  Client client(WINDOW_WIDTH, WINDOW_HEIGHT);
  client.run();

  exit(EXIT_SUCCESS);
}
