#include <GL/freeglut.h>
#include "terrain.h"
#include "utils.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Camera variables
float camX = 0.0f, camY = 50.0f, camZ = 200.0f; // position
float yaw = -90.0f;   // rotation left/right
float pitch = 0.0f; // rotation up/down
float moveSpeed = 5.0f;
float mouseSensitivity = 0.2f;

int lastMouseX, lastMouseY;
bool firstMouse = true;


// Function prototypes
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void timer(int value);

// Lighting setup
void setupLighting() {
    GLfloat light_pos[] = { 50.0f, 100.0f, 50.0f, 1.0f };
    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

// Initialization
void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);   // sky color
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    //glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    setupLighting();
    buildHeights();
    initRandom();

    // --- NEW CODE: Projection & Camera Setup ---
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 1.33, 0.1, 2000.0); // wide view

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set camera position to view full terrain
    gluLookAt(
        TERR_SIZE / 2.0f, 80.0f, TERR_SIZE * 1.5f,  // eye position
        TERR_SIZE / 2.0f, 0.0f, TERR_SIZE / 2.0f,   // look at center
        0.0f, 1.0f, 0.0f                            // up vector
    );
}


void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 1000.0);
}

// Basic camera keyboard controls (WASD)
//void keyboard(unsigned char key, int, int) {
//    switch (key) {
//    case 'w': camZ -= 2; lookZ -= 2; break;
//    case 's': camZ += 2; lookZ += 2; break;
//    case 'a': camX -= 2; lookX -= 2; break;
//    case 'd': camX += 2; lookX += 2; break;
//    case 'r': camY += 2; break;
//    case 'f': camY -= 2; break;
//    case 27: exit(0); // ESC
//    }
//}

void updateCamera(float& dirX, float& dirY, float& dirZ) {
    dirX = cosf(yaw * M_PI / 180.0f) * cosf(pitch * M_PI / 180.0f);
    dirY = sinf(pitch * M_PI / 180.0f);
    dirZ = sinf(yaw * M_PI / 180.0f) * cosf(pitch * M_PI / 180.0f);
}

void getCameraDirection(float& dirX, float& dirY, float& dirZ) {
    dirX = cosf(yaw * M_PI / 180.0f) * cosf(pitch * M_PI / 180.0f);
    dirY = sinf(pitch * M_PI / 180.0f);
    dirZ = sinf(yaw * M_PI / 180.0f) * cosf(pitch * M_PI / 180.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float dirX, dirY, dirZ;
    getCameraDirection(dirX, dirY, dirZ);

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f
    );

    drawTerrain();
    drawWater();

    glutSwapBuffers();
}


void mouseMotion(int x, int y) {
    if (firstMouse) {
        lastMouseX = x;
        lastMouseY = y;
        firstMouse = false;
    }

    int dx = x - lastMouseX;
    int dy = lastMouseY - y; // invert Y for natural feel
    lastMouseX = x;
    lastMouseY = y;

    yaw += dx * mouseSensitivity;
    pitch += dy * mouseSensitivity;

    // limit pitch to avoid flipping
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
    float dirX, dirY, dirZ;
    getCameraDirection(dirX, dirY, dirZ);

    // Normalize the direction for consistent speed
    float len = sqrtf(dirX * dirX + dirY * dirY + dirZ * dirZ);
    dirX /= len; dirY /= len; dirZ /= len;

    // Right vector (perpendicular to direction)
    float rightX = sinf((yaw + 90.0f) * M_PI / 180.0f);
    float rightZ = -cosf((yaw + 90.0f) * M_PI / 180.0f);

    switch (key) {
    case 'w': camX += dirX * moveSpeed; camY += dirY * moveSpeed; camZ += dirZ * moveSpeed; break;
    case 's': camX -= dirX * moveSpeed; camY -= dirY * moveSpeed; camZ -= dirZ * moveSpeed; break;
    case 'a': camX -= rightX * moveSpeed; camZ -= rightZ * moveSpeed; break;
    case 'd': camX += rightX * moveSpeed; camZ += rightZ * moveSpeed; break;
    case 'q': camY -= moveSpeed; break;
    case 'e': camY += moveSpeed; break;
    case 27: exit(0); break; // ESC
    }
    glutPostRedisplay();
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}



int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Procedural Terrain Generator");

    init();
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);
    glutPassiveMotionFunc(mouseMotion); // rotate by mouse move

    glutMainLoop();
    return 0;
}
