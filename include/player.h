#ifndef PLAYER_H
#define PLAYER_H

#define PLAYER_SPEED 100.f
#define FOV M_PI / 3.f
#define SLOW_SPEED_MULTPILIER 0.6f
#define FRICTION 0.6f;

#include <SDL2/SDL.h>

#include "tilemap.h"

typedef struct {
  float x, y;
  float vx, vy;
  float angle;
} Player;

void player_update(Player* p, float deltaTime, Tilemap *t);

#endif
