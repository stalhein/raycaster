#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>


#define SCR_WIDTH 800
#define SCR_HEIGHT 600

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  uint32_t *buffer;
  SDL_Texture *texture;
} Renderer;

typedef struct {
  uint32_t *texture;
  int width, height;
} Texture;

Renderer *renderer_create();
void renderer_clear(Renderer *r);
void renderer_update(Renderer *r);
void renderer_destroy(Renderer *r);

void renderer_draw_column(Renderer *r, int x, float perpendicularDistance,
                          float wallDist, Texture* texture, int side);

Texture *texture_load(const char *path, int width, int height);
void texture_destroy(Texture *t);

#endif
