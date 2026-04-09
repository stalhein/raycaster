#ifndef TILEMAP_H
#define TILEMAP_H

#include <math.h>
#include <stdint.h>

#define MAP_SIZE 8
#define TILE_SIZE 32

typedef struct {
  int tileX, tileY;
  float distance;
  int side;
  float wallDist;
} Hit;

Hit raycast(float startX, float startY, float directionX, float directionY);

#endif
