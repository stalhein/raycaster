#ifndef WORLD_H
#define WORLD_H

#include <SDL2/SDL.h>

#include "lights.h"
#include "player.h"
#include "renderer.h"
#include "tilemap.h"

#include "settings.h"

#define NUMBER_TEXTURES 3

typedef struct {
  Renderer *renderer;
  Tilemap *tileMap;
  Player player;
  LightManager lightManager;
  Texture *textures[NUMBER_TEXTURES];
} World;

World *world_init(SDL_Window *window, const char* worldPath);

void world_update(World *w, float deltaTime);

void world_render(World *w);

void world_process_mouse(World *w, SDL_Event *e);

void world_destroy(World *w);

#endif
