#ifndef TILEMAP_H
#define TILEMAP_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#define MAP_SIZE 8
#define TILE_SIZE 32

typedef struct {
  int width, height;
  uint8_t *map;
} Tilemap;

typedef struct {
  int tileX, tileY;
  float distance;
  int side;
  float wallDist;
} Hit;

Tilemap* tilemap_create(const char* path);

Hit raycast(Tilemap* t, float startX, float startY, float directionX, float directionY);

#endif
