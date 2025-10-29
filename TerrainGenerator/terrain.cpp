#include <GL/freeglut.h>
#include <cmath>
#include "terrain.h"
#include "perlin.h"

float heightMap[TERR_SIZE][TERR_SIZE];
static Perlin perlin;

// Multi-octave Perlin noise function (fBm)
float octaveNoise(float x, float z, int octaves, float persistence, float baseFreq) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f; // for normalization
    float frequency = baseFreq;

    for (int i = 0; i < octaves; i++) {
        total += perlin.noise(x * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence; // reduce amplitude each octave
        frequency *= 2.0f;        // double frequency each octave
    }
    return total / maxValue; // normalize to 0..1
}

float getHeight(int x, int z) {
    // tweakable parameters
    int octaves = 6;          // number of layers (2–8 is common)
    float persistence = 0.5f; // how much each octave contributes (0.3–0.7)
    float baseFreq = 0.008f;   // base terrain size
    float amp = AMPLITUDE;    // overall height scaling

    float n = octaveNoise(x, z, octaves, persistence, baseFreq);
    return amp * (n - 0.5f) * 2.0f; // shift noise to -amp..+amp
}


void buildHeights() {
    for (int x = 0; x < TERR_SIZE; x++)
        for (int z = 0; z < TERR_SIZE; z++)
            heightMap[x][z] = getHeight(x, z);
}

// Draw vertical side walls along terrain edges
void drawTerrainSides() {
    float baseY = WATER_LEVEL;  // where the sides connect to
    glColor3f(0.4f, 0.3f, 0.2f); // brown rock color

    glBegin(GL_QUADS);

    // --- Front edge (z = 0) ---
    for (int x = 0; x < TERR_SIZE - 1; x++) {
        glVertex3f(x, baseY, 0);
        glVertex3f(x + 1, baseY, 0);
        glVertex3f(x + 1, heightMap[x + 1][0], 0);
        glVertex3f(x, heightMap[x][0], 0);
    }

    // --- Back edge (z = TERR_SIZE - 1) ---
    for (int x = 0; x < TERR_SIZE - 1; x++) {
        glVertex3f(x, baseY, TERR_SIZE - 1);
        glVertex3f(x + 1, baseY, TERR_SIZE - 1);
        glVertex3f(x + 1, heightMap[x + 1][TERR_SIZE - 1], TERR_SIZE - 1);
        glVertex3f(x, heightMap[x][TERR_SIZE - 1], TERR_SIZE - 1);
    }

    // --- Left edge (x = 0) ---
    for (int z = 0; z < TERR_SIZE - 1; z++) {
        glVertex3f(0, baseY, z);
        glVertex3f(0, baseY, z + 1);
        glVertex3f(0, heightMap[0][z + 1], z + 1);
        glVertex3f(0, heightMap[0][z], z);
    }

    // --- Right edge (x = TERR_SIZE - 1) ---
    for (int z = 0; z < TERR_SIZE - 1; z++) {
        glVertex3f(TERR_SIZE - 1, baseY, z);
        glVertex3f(TERR_SIZE - 1, baseY, z + 1);
        glVertex3f(TERR_SIZE - 1, heightMap[TERR_SIZE - 1][z + 1], z + 1);
        glVertex3f(TERR_SIZE - 1, heightMap[TERR_SIZE - 1][z], z);
    }

    glEnd();
}

// Optional: close the bottom with a flat quad
void drawTerrainBottom() {
    glColor3f(0.25f, 0.2f, 0.15f);
    glBegin(GL_QUADS);
    glVertex3f(0, WATER_LEVEL, 0);
    glVertex3f(TERR_SIZE, WATER_LEVEL, 0);
    glVertex3f(TERR_SIZE, WATER_LEVEL, TERR_SIZE);
    glVertex3f(0, WATER_LEVEL, TERR_SIZE);
    glEnd();
}


void drawTerrain() {
    glPushMatrix();
    glTranslatef(-TERR_SIZE / 2.0f, 3.0f, -TERR_SIZE / 2.0f);


    // --- Top surface (main terrain) ---
    for (int x = 0; x < TERR_SIZE - 1; x++) {
        glBegin(GL_TRIANGLE_STRIP);
        for (int z = 0; z < TERR_SIZE; z++) {
            float h1 = heightMap[x][z];
            float h2 = heightMap[x + 1][z];

            // color by elevation
            if (h1 < WATER_LEVEL) glColor3f(0.2f, 0.4f, 1.0f);        // water
            else if (h1 < 2.0f) glColor3f(0.1f, 0.7f, 0.1f);          // grass
            else if (h1 < 4.0f) glColor3f(0.5f, 0.4f, 0.3f);          // rock
            else glColor3f(0.9f, 0.9f, 0.9f);                         // snow

            glVertex3f(x, h1, z);
            glVertex3f(x + 1, h2, z);
        }
        glEnd();
    }

    // --- Side walls of the terrain ---
    float baseY = WATER_LEVEL;
    glColor3f(0.4f, 0.3f, 0.2f); // cliff/rock color
    glBegin(GL_QUADS);

    // Front edge (z = 0)
    for (int x = 0; x < TERR_SIZE - 1; x++) {
        glVertex3f(x, baseY, 0);
        glVertex3f(x + 1, baseY, 0);
        glVertex3f(x + 1, heightMap[x + 1][0], 0);
        glVertex3f(x, heightMap[x][0], 0);
    }

    // Back edge (z = TERR_SIZE - 1)
    for (int x = 0; x < TERR_SIZE - 1; x++) {
        glVertex3f(x, baseY, TERR_SIZE - 1);
        glVertex3f(x + 1, baseY, TERR_SIZE - 1);
        glVertex3f(x + 1, heightMap[x + 1][TERR_SIZE - 1], TERR_SIZE - 1);
        glVertex3f(x, heightMap[x][TERR_SIZE - 1], TERR_SIZE - 1);
    }

    // Left edge (x = 0)
    for (int z = 0; z < TERR_SIZE - 1; z++) {
        glVertex3f(0, baseY, z);
        glVertex3f(0, baseY, z + 1);
        glVertex3f(0, heightMap[0][z + 1], z + 1);
        glVertex3f(0, heightMap[0][z], z);
    }

    // Right edge (x = TERR_SIZE - 1)
    for (int z = 0; z < TERR_SIZE - 1; z++) {
        glVertex3f(TERR_SIZE - 1, baseY, z);
        glVertex3f(TERR_SIZE - 1, baseY, z + 1);
        glVertex3f(TERR_SIZE - 1, heightMap[TERR_SIZE - 1][z + 1], z + 1);
        glVertex3f(TERR_SIZE - 1, heightMap[TERR_SIZE - 1][z], z);
    }

    glEnd();

    // --- Bottom face (close it off) ---
    glColor3f(0.25f, 0.2f, 0.15f);
    glBegin(GL_QUADS);
    glVertex3f(0, baseY, 0);
    glVertex3f(TERR_SIZE, baseY, 0);
    glVertex3f(TERR_SIZE, baseY, TERR_SIZE);
    glVertex3f(0, baseY, TERR_SIZE);
    glEnd();

    glPopMatrix();
}

void drawWater() {
    glPushMatrix();
    glTranslatef(-TERR_SIZE / 2.0f, -0.5f, -TERR_SIZE / 2.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.4f, 0.9f, 1.0f);

    glBegin(GL_QUADS);
    glVertex3f(0, WATER_LEVEL, 0);
    glVertex3f(TERR_SIZE, WATER_LEVEL, 0);
    glVertex3f(TERR_SIZE, WATER_LEVEL, TERR_SIZE);
    glVertex3f(0, WATER_LEVEL, TERR_SIZE);
    glEnd();
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
}
