#include "lights.h"

LightManager *lightmanager_init(int size) {
  LightManager manager;
  manager.capacity = size;
  manager.count = 0;
  manager.lights = malloc(size * sizeof(Light));
}


void lightmanager_add_light(LightManager * m, float x, float y, float strength) {
  if (m->count >= m->capacity) {
    m->capacity += 4;
    m->lights = realloc(m->lights, m->capacity * sizeof(Light));
  }

  m->lights[m->count] = (Light){x, y, strength};
  m->count++;
}


void lightmanager_destroy(LightManager *m) {
  free(m->lights);
  m->lights = NULL;
  m->count = 0;
  m->capacity = 0;
}


