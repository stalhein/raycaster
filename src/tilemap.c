#include "tilemap.h"

Tilemap *tilemap_create(const char* path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
    perror("Failed to open file.");
    return NULL;
  }

  int width, height;
  fread(&width, sizeof(int), 1, f);
  fread(&height, sizeof(int), 1, f);

  Tilemap *tilemap = malloc(sizeof(Tilemap));

  tilemap->map = malloc(width * height);
  fread(tilemap->map, sizeof(uint8_t), width*height, f);

  fclose(f);
  tilemap->width = width;
  tilemap->height = height;

  return tilemap;
}

Hit raycast(Tilemap *t, float startX, float startY, float directionX,
            float directionY) {
  float posX = startX / TILE_SIZE;
  float posY = startY / TILE_SIZE;

  int tileX = (int)posX;
  int tileY = (int)posY;

  float deltaDistanceX = (directionX == 0) ? 1e30 : fabs(1.f / directionX);
  float deltaDistanceY = (directionY == 0) ? 1e30 : fabs(1.f / directionY);

  float sideDistanceX, sideDistanceY;
  int stepX, stepY;

  if (directionX < 0) {
    stepX = -1;
    sideDistanceX = (posX - tileX) * deltaDistanceX;
  } else {
    stepX = 1;
    sideDistanceX = (tileX + 1.f - posX) * deltaDistanceX;
  }

  if (directionY < 0) {
    stepY = -1;
    sideDistanceY = (posY - tileY) * deltaDistanceY;
  } else {
    stepY = 1;
    sideDistanceY = (tileY + 1.f - posY) * deltaDistanceY;
  }

  int side;
  int hit = 0;

  while (!hit) {
    if (sideDistanceX < sideDistanceY) {
      sideDistanceX += deltaDistanceX;
      tileX += stepX;
      side = 0;
    } else {
      sideDistanceY += deltaDistanceY;
      tileY += stepY;
      side = 1;
    }

    if (tileX < 0 || tileX >= t->width || tileY < 0 || tileY >= t->height) {
      break;
    }

    if (t->map[tileY * t->width + tileX] > 0) {
      hit = 1;
    }
  }

  float perpendicularDistance;
  if (side == 0)
    perpendicularDistance = (tileX - posX + (1 - stepX) / 2.f) / directionX;
  else
    perpendicularDistance = (tileY - posY + (1 - stepY) / 2.f) / directionY;

  float wallX;
  if (side == 0)
    wallX = posY + perpendicularDistance * directionY;
  else
    wallX = posX + perpendicularDistance * directionX;

  wallX -= floor(wallX);

  return (Hit){tileX, tileY, perpendicularDistance, side, wallX};
}
