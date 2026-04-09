#include "player.h"

void player_update(Player* p, float deltaTime) {
  const Uint8* keys = SDL_GetKeyboardState(NULL);

  float c = cos(p->angle);
  float s = sin(p->angle);

  float ax = 0.f, ay = 0.f;

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

  p->vx += ax;
  p->vy += ay;

  p->vx *= FRICTION;
  p->vy *= FRICTION;

  float ss = p->vx*p->vx+p->vy*p->vy;
  if (ss > PLAYER_SPEED*PLAYER_SPEED) {
    float speed = sqrtf(ss);
    p->vx *= PLAYER_SPEED / speed;
    p->vy *= PLAYER_SPEED / speed;
  }

  p->x += p->vx;
  p->y += p->vy;
}
