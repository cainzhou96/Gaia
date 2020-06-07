#include "main.h"

#ifndef __APPLE__
#include <SDL.h>
#undef main
#endif


#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(void) {
  // Create the client.
  Client client(WINDOW_WIDTH, WINDOW_HEIGHT);
  client.run();

  exit(EXIT_SUCCESS);
}
