#ifndef LIGHTS_H
#define LIGHTS_H

#include <stdlib.h>

typedef struct {
  float x, y, strength;
} Light;

typedef struct {
  int count;
  int capacity;
  Light *lights;
} LightManager;

LightManager *lightmanager_init(int size);
void lightmanager_add_light(LightManager * m, float x, float y, float strength);
void lightmanager_destroy(LightManager *m);

#endif
