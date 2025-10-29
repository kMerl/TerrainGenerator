#include "terrain.h"7

// Draw a simple tree using cylinders and spheres
void drawTree(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.4, 0.25, 0.1);
    //glutSolidCylinder(0.1, 1.0, 8, 8);  // trunk

    glTranslatef(0.0f, 1.0f, 0.0f);
    glColor3f(0.0f, 0.8f, 0.0f);
    glutSolidSphere(0.5f, 8.0f, 8.0f);         // canopy
    glPopMatrix();
}

// Randomly scatter trees based on terrain height
void scatterTrees() {
    for (int i = 0; i < 80; i++) {
        int x = rand() % TERR_SIZE;
        int z = rand() % TERR_SIZE;
        float h = heightMap[x][z];
        if (h > 0 && h < 4) drawTree(x, h, z);
    }
}
