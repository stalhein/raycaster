#include "world.h"

const char *TEXTURE_PATHS[NUMBER_TEXTURES] = {
    "assets/wall5.png", "assets/floor3.png", "assets/ceiling.png"};

World *world_init(SDL_Window *window, const char *worldPath) {
  World *w = malloc(sizeof(World));
  if (!w) return NULL;

  w->renderer = renderer_create(window);
  if (!w->renderer) {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return NULL;
  }

  for (int i = 0; i < NUMBER_TEXTURES; ++i) {
    w->textures[i] = texture_load(TEXTURE_PATHS[i], 64, 64);
    if (!w->textures[i]) {
      printf("Failed to load texture: %s\n", TEXTURE_PATHS[i]);
      return NULL;
    }
  }

  w->player = (Player){100.f, 100.f, 0.f, 0.f, 0.f};

  w->tileMap = tilemap_create(worldPath);

  return w;
}

void world_update(World *w, float deltaTime) {
  player_update(&w->player, deltaTime, w->tileMap);
}

void world_render(World *w) {
  renderer_clear(w->renderer);

  float dirX = cos(w->player.angle);
  float dirY = sin(w->player.angle);
  float planeX = -dirY * tan(FOV / 2.f);
  float planeY = dirX * tan(FOV / 2.f);

  // Floor
  float x0 = dirX - planeX;
  float y0 = dirY - planeY;
  float x1 = dirX + planeX;
  float y1 = dirY + planeY;

  const int horizon = SCR_HEIGHT / 2;
  for (int y = horizon + 1; y < SCR_HEIGHT; ++y) {
    float distance = (float)SCR_HEIGHT / (2.f * y - SCR_HEIGHT);

    distance *= TILE_SIZE;

    float sX = distance * (x1 - x0) / SCR_WIDTH;
    float sY = distance * (y1 - y0) / SCR_WIDTH;

    float floorX = w->player.x + distance * x0;
    float floorY = w->player.y + distance * y0;

    for (int x = 0; x < SCR_WIDTH; ++x) {
      float scaledX = floorX / TILE_SIZE;
      float scaledY = floorY / TILE_SIZE;

      scaledX -= floorf(scaledX);
      scaledY -= floorf(scaledY);

      int textureX = ((int)(scaledX * w->textures[1]->width));
      int textureY = ((int)(scaledY * w->textures[1]->height));

      if (textureX < 0)
        textureX = 0;
      if (textureY < 0)
        textureY = 0;

      textureX %= w->textures[1]->width;
      textureY %= w->textures[1]->height;

      Uint32 colour =
          w->textures[1]->texture[textureY * w->textures[1]->width + textureX];
      Uint8 cr = ((colour >> 16) & 0xFF);
      Uint8 cg = ((colour >> 8) & 0xFF);
      Uint8 cb = (colour & 0xFF);
      colour = (cr << 16) | (cg << 8) | cb;

      w->renderer->buffer[y * SCR_WIDTH + x] = colour;

      int ceilingY = SCR_HEIGHT - y - 1;

      colour =
          w->textures[2]->texture[textureY * w->textures[2]->width + textureX];
      cr = ((colour >> 16) & 0xFF);
      cg = ((colour >> 8) & 0xFF);
      cb = (colour & 0xFF);
      colour = (cr << 16) | (cg << 8) | cb;
      w->renderer->buffer[ceilingY * SCR_WIDTH + x] = colour;

      floorX += sX;
      floorY += sY;
    }
  }

  // Walls
  for (int i = 0; i < SCR_WIDTH; ++i) {
    float x = 2.f * i / (float)SCR_WIDTH - 1.f;
    float dx = dirX + planeX * x;
    float dy = dirY + planeY * x;
    Hit hit = raycast(w->tileMap, w->player.x, w->player.y, dx, dy);
    renderer_draw_column(w->renderer, i, hit.distance, hit.wallDist, w->textures[0],
                         hit.side);
  }

  renderer_update(w->renderer);
}

void world_process_mouse(World *w, SDL_Event *e) {
  w->player.angle += e->motion.xrel * MOUSE_SENSITIVITY;

  if (w->player.angle > 2 * M_PI)
    w->player.angle -= 2 * M_PI;
  else if (w->player.angle < 0)
    w->player.angle += 2 * M_PI;
}

void world_destroy(World *w) {
  for (int i = 0; i < NUMBER_TEXTURES; ++i) {
    texture_destroy(w->textures[i]);
  }

  renderer_destroy(w->renderer);

  free(w);
}
