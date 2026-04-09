#include "player.h"

void player_update(Player* p, float deltaTime, Tilemap* t) {
  const Uint8* keys = SDL_GetKeyboardState(NULL);

  float c = cos(p->angle);
  float s = sin(p->angle);

  float ax = 0.f, ay = 0.f;
  float vx = p->vx, vy = p->vy;
  float x = p->x, y = p->y;

  if (keys[SDL_SCANCODE_W]) {
    ax += c * PLAYER_SPEED * deltaTime;
    ay += s * PLAYER_SPEED * deltaTime;
  }
  if (keys[SDL_SCANCODE_S]) {
    ax -= c * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
    ay -= s * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
  }
  if (keys[SDL_SCANCODE_A]) {
    ax += s * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
    ay -= c * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
  }
  if (keys[SDL_SCANCODE_D]) {
    ax -= s * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
    ay += c * PLAYER_SPEED * deltaTime * SLOW_SPEED_MULTPILIER;
  }

  vx += ax;
  vy += ay;

  vx *= FRICTION;
  vy *= FRICTION;

  float ss = vx*vx+vy*vy;
  if (ss > PLAYER_SPEED*PLAYER_SPEED) {
    float speed = sqrtf(ss);
    vx *= PLAYER_SPEED / speed;
    vy *= PLAYER_SPEED / speed;
  }

  x += vx;

  int minTileX = (int)((x-5) / TILE_SIZE);
  int minTileY = (int)((y-5) / TILE_SIZE);
  int maxTileX = (int)((x+5) / TILE_SIZE);
  int maxTileY = (int)((y+5) / TILE_SIZE);

  int collided = 0;

  for (int ty = minTileY; ty <= maxTileY; ++ty) {
    for (int tx = minTileX; tx <= maxTileX; ++tx) {
      if (tx < 0 || tx >= t->width || ty < 0 || ty >= t->height) {
        collided = 1;
        break;
      }

      if (t->map[ty * t->width + tx] != 0) {
        collided = 1;
        break;
      }
    }
    if (collided) break;
  }

  if (collided) {
    x -= vx;
    p->vx = 0;
  } else {
    p->vx = vx;
  }

  y += vy;

  minTileX = (int)((x-5) / TILE_SIZE);
  minTileY = (int)((y-5) / TILE_SIZE);
  maxTileX = (int)((x+5) / TILE_SIZE);
  maxTileY = (int)((y+5) / TILE_SIZE);

  collided = 0;

  for (int ty = minTileY; ty <= maxTileY; ++ty) {
    for (int tx = minTileX; tx <= maxTileX; ++tx) {
      if (tx < 0 || tx >= t->width || ty < 0 || ty >= t->height) {
        collided = 1;
        break;
      }

      if (t->map[ty * t->width + tx] != 0) {
        collided = 1;
        break;
      }
    }
    if (collided) break;
  }

  if (collided) {
    y -= vy;
    p->vy = 0;
  } else {
    p->vy = vy;
  }

  p->x = x;
  p->y = y;
}
