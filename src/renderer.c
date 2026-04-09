#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_resize2.h"

Renderer *renderer_create() {
  Renderer *r = malloc(sizeof(Renderer));

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialize SDL2\n%c\n", SDL_GetError());
    return NULL;
  }

  r->window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH, SCR_HEIGHT,
                               SDL_WINDOW_SHOWN);
  if (!r->window) {
    printf("Could not create window\n%c\n", SDL_GetError());
    SDL_Quit();
    return NULL;
  }

  SDL_SetRelativeMouseMode(SDL_TRUE);

  r->renderer = SDL_CreateRenderer(r->window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!r->renderer) {
    printf("Could not create renderer\n%c\n", SDL_GetError());
    SDL_DestroyWindow(r->window);
    SDL_Quit();
    return NULL;
  }

  // Pixel buffer
  r->buffer = malloc(SCR_WIDTH * SCR_HEIGHT * sizeof(uint32_t));
  r->texture =
      SDL_CreateTexture(r->renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, SCR_WIDTH, SCR_HEIGHT);

  return r;
}

void renderer_clear(Renderer *r) {
  for (int i = 0; i < SCR_WIDTH * SCR_HEIGHT; ++i) {
    r->buffer[i] = 0xFFFFFFFF;
  }
}

void renderer_update(Renderer *r) {
  SDL_UpdateTexture(r->texture, NULL, r->buffer, SCR_WIDTH * sizeof(uint32_t));

  SDL_RenderCopy(r->renderer, r->texture, NULL, NULL);
  SDL_RenderPresent(r->renderer);
}

void renderer_destroy(Renderer *r) {
  SDL_DestroyTexture(r->texture);
  SDL_DestroyRenderer(r->renderer);
  SDL_DestroyWindow(r->window);
  SDL_Quit();
  free(r->buffer);
  free(r);
}

void renderer_draw_column(Renderer *r, int x, float perpendicularDistance,
                          float wallDist, Texture *texture, int side) {
  if (x < 0 || x >= SCR_WIDTH)
    return;
  int height = (int)(SCR_HEIGHT / perpendicularDistance);
  int start = (int)(SCR_HEIGHT / 2 - height / 2);
  int end = start + height;
  int textureX = (int)(wallDist * (float)texture->width);
  if (textureX < 0)
    textureX = 0;
  if (textureX >= texture->width)
    textureX = texture->width - 1;
  for (int y = start; y < end; ++y) {
    if (y < 0 || y >= SCR_HEIGHT)
      continue;
    int textureY = (int)((float)(y-start) / height * texture->height);
    if (textureY < 0) textureY = 0;
    if (textureY >= texture->height) textureY = texture->height-1;
    Uint32 colour = texture->texture[textureY * texture->width + textureX];
    float shade = 1.f / (1.f + perpendicularDistance * 0.1f);
    if (side == 1 && shade > 0.2f) shade -= 0.2f;
    Uint8 cr = ((colour >> 16) & 0xFF) * shade;
    Uint8 cg = ((colour >> 8) & 0xFF) * shade;
    Uint8 cb = (colour & 0xFF) * shade;
    colour = (cr << 16) | (cg << 8) | cb;
    r->buffer[y * SCR_WIDTH + x] = colour;
  }
}

Texture *texture_load(const char *path, int width, int height) {
  int w, h, channels;

  unsigned char *data = stbi_load(path, &w, &h, &channels, 4);

  if (data == NULL) {
    printf("Error loading image.\n%s\n", stbi_failure_reason());
    return NULL;
  }

  unsigned char *resizedData = (unsigned char *)malloc(width * height * 4);
  stbir_resize_uint8_linear(data, w, h, 0, resizedData, width, height, 0, 4);


  uint32_t *converted = malloc(width*height*sizeof(uint32_t));
  for (int i = 0; i < width * height; ++i) {
    unsigned char r = resizedData[i * 4 + 0];
    unsigned char g = resizedData[i * 4 + 1];
    unsigned char b = resizedData[i * 4 + 2];
    unsigned char a = resizedData[i * 4 + 3];
    converted[i] = (a << 24) | (r << 16) | (g << 8) | b;
  }

  free(resizedData);

  Texture *t = malloc(sizeof(Texture));

  t->texture = converted;
  t->width = width;
  t->height = height;

  stbi_image_free(data);
  return t;
}

void texture_destroy(Texture *t) { free(t); }
