#include <SDL2/SDL.h>

#include <math.h>
#include <stdio.h>

#include "world.h"

#define FLOORSCALE 32.f

int main() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialize SDL2\n%c\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH,
                                        SCR_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Could not create window\n%c\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  World *world = world_init(window, "worlds/1.wld");

  // Main loop
  SDL_Event event;
  Uint64 last = SDL_GetPerformanceCounter();
  float deltaTime = 0.f;
  int running = 1;
  while (running) {
    Uint64 now = SDL_GetPerformanceCounter();
    deltaTime =
        (float)(now - last) * 1000.f / SDL_GetPerformanceFrequency() * 0.001f;
    last = now;

    printf("FPS: %f\n", 1000 / deltaTime);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          running = 0;
        }
      } else if (event.type == SDL_MOUSEMOTION) {
        world_process_mouse(world, &event);
      }
    }

    world_update(world, deltaTime);

    world_render(world);
  }

  world_destroy(world);

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
