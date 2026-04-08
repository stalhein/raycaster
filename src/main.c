#include <SDL2/SDL.h>

#include <stdio.h>
#include <math.h>

const int SCR_WIDTH = 800, SCR_HEIGHT = 600;
const int NUMBER_PIXELS = SCR_WIDTH * SCR_HEIGHT;

const int MAP_SIZE = 8;
const int TILE_SIZE = 32;

const float FOV = M_PI / 3.f;
const int RAYS = 800;

typedef struct {
  int tileX, tileY;
  float distance;
  int side;
  float wallDist;
} Hit;


typedef struct {
  float x, y;
  float angle;
} Player;

void renderColumn(uint32_t *buffer, int startX, int startY, int height, uint32_t colour);
Hit raycast(uint8_t *tileMap, float startX,
            float startY, float directionX, float directionY);

int main() {
  // Init SDL2
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("Could not initialize SDL2\n%c\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Raycaster", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, SCR_WIDTH,
                                        SCR_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Could not create window\n%c\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    printf("Could not create renderer\n%c\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  // Pixel buffer
  uint32_t pixels[SCR_WIDTH * SCR_HEIGHT];
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, SCR_WIDTH, SCR_HEIGHT);

  // Tile map
  uint8_t TILE_MAP []= {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0,
      1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
      0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

  float playerX = 100.f, playerY = 100.f;
  float angle = M_PI / 4;
  float speed = 100.f;
  float turn = 3.f;
  // Main loop
  SDL_Event event;
  Uint64 last = SDL_GetPerformanceCounter();
  float deltaTime = 0.f;
  int running = 1;
  while (running) {
    Uint64 now = SDL_GetPerformanceCounter();
    deltaTime = (float)(now - last) * 1000.f /
                SDL_GetPerformanceFrequency() * 0.001f;
    last = now;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_W]) {
      playerX += cos(angle) * speed * deltaTime;
      playerY += sin(angle) * speed * deltaTime;
    }
    if (keys[SDL_SCANCODE_S]) {
      playerX -= cos(angle) * speed * deltaTime;
      playerY -= sin(angle) * speed * deltaTime;
    }
    if (keys[SDL_SCANCODE_A]) {
      angle -= turn * deltaTime;
    }
    if (keys[SDL_SCANCODE_D]) {
      angle += turn * deltaTime;
    }

    for (int i = 0; i < NUMBER_PIXELS; ++i) {
      pixels[i] = 0xFFFFFFFF;
    }

    float dirX = cos(angle);
    float dirY = sin(angle);
    float planeX = -dirY * tan(FOV / 2.f);
    float planeY = dirX * tan(FOV / 2.f);
    for (int i = 0; i < RAYS; ++i) {
      float x = 2.f * i / (float)RAYS - 1.f;
      float dx = dirX + planeX * x;
      float dy = dirY + planeY * x;
      Hit hit = raycast(TILE_MAP, playerX, playerY, dx, dy);
      int height = (int)(SCR_HEIGHT / hit.distance);
      renderColumn(pixels, i, SCR_HEIGHT/2 - height / 2, height, 0xFFFFF000);
    }

    SDL_UpdateTexture(texture, NULL, pixels, SCR_WIDTH * sizeof(uint32_t));

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

void renderColumn(uint32_t *buffer, int startX, int startY, int height, uint32_t colour) {
  for (int y = startY; y < startY + height; ++y) {
    if (startX < 0 || y < 0 || startX >= SCR_WIDTH || y >= SCR_HEIGHT) continue;
    buffer[y * SCR_WIDTH + startX] = colour;
  }
}

Hit raycast(uint8_t *tileMap, float startX,
            float startY, float directionX, float directionY) {
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

    if (tileX < 0 || tileX >= MAP_SIZE || tileY < 0 || tileY >= MAP_SIZE) {
      break;
    }

    if (tileMap[tileY * MAP_SIZE + tileX] > 0) {
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
