#include <SDL2/SDL.h>

#include <math.h>
#include <stdio.h>

#include "player.h"
#include "renderer.h"
#include "tilemap.h"

#define FLOORSCALE 64.f
#define MOUSE_SENSITIVITY 0.006f

int main() {
  Renderer *renderer = renderer_create();
  if (!renderer) {
    return 1;
  }

  Texture *wallTexture = texture_load("assets/wall5.png", 64, 64);
  Texture *floorTexture = texture_load("assets/floor3.png", 64, 64);
  Texture *ceilingTexture = texture_load("assets/ceiling.png", 64, 64);
  if (!wallTexture || !floorTexture || !ceilingTexture) {
    printf("Failed to load textures.");
    return 1;
  }

  Player player = {100.f, 100.f, 0.f, 0.f, 0.f};

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

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      } else if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
          running = 0;
        }
      } else if (event.type == SDL_MOUSEMOTION) {
        player.angle += event.motion.xrel * MOUSE_SENSITIVITY;

        if (player.angle > 2 * M_PI) player.angle -= 2 * M_PI;
        else if (player.angle < 0) player.angle += 2 * M_PI;
      }
    }

    player_update(&player, deltaTime);

    renderer_clear(renderer);

    float dirX = cos(player.angle);
    float dirY = sin(player.angle);
    float planeX = -dirY * tan(FOV / 2.f);
    float planeY = dirX * tan(FOV / 2.f);

    // Floor
    float x0 = dirX - planeX;
    float y0 = dirY - planeY;
    float x1 = dirX + planeX;
    float y1 = dirY + planeY;

    const int horizon = SCR_HEIGHT / 2;
    for (int y = horizon + 1; y < SCR_HEIGHT; ++y) {
      float distance = (float)(horizon) / (float)(y - horizon);

      distance *= TILE_SIZE;

      float sX = distance * (x1 - x0) / SCR_WIDTH;
      float sY = distance * (y1 - y0) / SCR_WIDTH;

      float floorX = player.x + distance * x0;
      float floorY = player.y + distance * y0;

      for (int x = 0; x < SCR_WIDTH; ++x) {
        float scaledX = floorX / FLOORSCALE;
        float scaledY = floorY / FLOORSCALE;

        scaledX -= floorf(scaledX);
        scaledY -= floorf(scaledY);

        int textureX = ((int)(scaledX * floorTexture->width));
        int textureY = ((int)(scaledY * floorTexture->height));

        if (textureX < 0)
          textureX = 0;
        if (textureY < 0)
          textureY = 0;

        textureX %= floorTexture->width;
        textureY %= floorTexture->height;

        renderer->buffer[y * SCR_WIDTH + x] =
            floorTexture->texture[textureY * floorTexture->width + textureX];

        int ceilingY = SCR_HEIGHT - y-1;

        renderer->buffer[ceilingY * SCR_WIDTH + x] =
            ceilingTexture->texture[textureY * floorTexture->width + textureX];

        floorX += sX;
        floorY += sY;
      }
    }

    // Walls
    for (int i = 0; i < SCR_WIDTH; ++i) {
      float x = 2.f * i / (float)SCR_WIDTH - 1.f;
      float dx = dirX + planeX * x;
      float dy = dirY + planeY * x;
      Hit hit = raycast(player.x, player.y, dx, dy);
      renderer_draw_column(renderer, i, hit.distance, hit.wallDist,
                           wallTexture, hit.side);
    }

    renderer_update(renderer);
  }

  texture_destroy(wallTexture);
  texture_destroy(floorTexture);
  texture_destroy(ceilingTexture);
  renderer_destroy(renderer);

  return 0;
}
