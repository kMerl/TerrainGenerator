#pragma once
#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// Terrain constants
const int TERR_SIZE = 400;      // terrain grid size
const float SCALE = 0.2f;  // controls frequency of hills
const float AMPLITUDE = 25; // controls height
const float WATER_LEVEL = -0.5f;

// height map array
extern float heightMap[TERR_SIZE][TERR_SIZE];

// Core functions
void buildHeights();
float getHeight(int x, int z);
void drawTerrain();
void drawWater();

#endif
