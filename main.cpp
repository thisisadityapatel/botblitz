#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GLUT/glut.h>
#include "robot.h"
#else
#include <windows.h>
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <unistd.h> // For usleep

struct Vertex
{
    float x, y, z;
};


struct Face
{
    int v1, v2, v3;
};

struct Ball {
    float x, y, z;
    float dx, dy, dz;
    float lifespan;
};

struct RobotComponent
{
    std::vector<Bullet> bullets;                                     // Robot bullets
    GLdouble spinRobotArmValue = 0.0;                                // Robot arm spin value
    GLdouble spinRobotCannonValue = 0.0;                             // Robot cannon spin value
    GLUquadric *quadric = gluNewQuadric();                           // Quadric pointer for rendering
    GLint steppingstage = 0;                                         // Stepping stage (0, 1, 2, 3)
    float upperLegAngle = 0.0f;                                      // Upper leg angle
    float lowerLegAngle = 0.0f;                                      // Lower leg angle
    float alternateLegRotate = 1.0f;                                 // Alternating leg rotation
    RobotInitialPosition robotStartingPosition = {0.0f, 0.0f, 0.0f}; // Initial position
    float stillalive = true;                                         // Robot Current Status true = Alive false = Dead
    float afterDeadBodyAngle = 0;                                    // Max 30
};

struct Robot_X_Z_Position{
    GLdouble x = 0.0;
    GLdouble z = 0.0;
};

void display(void);
void initMouse();
void drawGround(void);
void drawRobots(void);
void drawBalls();
void fireCannonBall();
void ballUpdateTimer(int value);
void updateBalls(float deltaTime);
void reshape(int w, int h);
void mouseHandler(int x, int y);
void mouseButtonHandler(int button, int state, int x, int y);
bool loadOBJ(const char *filePath);
void initOpenGL(int vHeight, int vWidth);
void keyboardHandler(unsigned char key, int x, int y);
void getCameraForwardVector(float &fx, float &fy, float &fz);
void drawCannonRelativeToCamera(void);
void drawBulletPoint(const std::vector<Bullet> &bullets);
void animateRobot(int val);
void drawRobotMesh(void);
void updateBullets(std::vector<Bullet> &bullets, float deltaTime);
void fireBulletTimer(int value);
void fireBullet(std::vector<Bullet> &bullets, float cannonX, float cannonY, float cannonZ, float cannonDX, float cannonDY, float cannonDZ, float sidewaysAngle);
void updateRobotPosition(int value);
bool allRobotsDeadStatus();
GLuint loadTexture(const char *filepath);

float groundSize = 100;
float groundLength = 100;
const int vWidth = 1080;  // Viewport width in pixels
const int vHeight = 1080; // Viewport height in pixels

float cameraX = 0.0f, cameraY = 1.50f, cameraZ = 15.0f; // Camera position
float cameraYaw = 40.5f, cameraPitch = 0.9f;           // Yaw and pitch angles
float cameraSpeed = 0.1f;                               // Movement speed
float sensitivity = 0.2f;                               // Mouse sensitivity

float cannonYaw = 0.0f;   // Cannon's independent yaw (horizontal rotation)
float cannonPitch = 0.0f; // Cannon's independent pitch (vertical rotation)

float rotationAngle = 90.0f; // Angle in degrees
float rotationAxisX = 1.0f;  // X-axis component of rotation axis
float rotationAxisY = 0.0f;  // Y-axis component of rotation axis
float rotationAxisZ = 0.0f;  // Z-axis component of rotation axis

bool isFreeLookEnabled = true;

float difficultyMultiplier = 1.0f; // Default to "Easy" (1x speed)

std::vector<Vertex> vertices;
std::vector<Face> faces;
std::vector<Ball> activeBalls;

// -------------------------------------
// ###### COMMON ROBOT VARIABLES ######
// -------------------------------------

GLdouble speedFactor = 2.4f;
GLdouble maxAngle = 35.0f;
GLfloat emissiveColor[] = {1.0, 0.0, 0.0, 1.0}; // Red emissive light
GLfloat haloColor[] = {1.0, 0.0, 0.0, 0.3};     // Transparent red halo
GLfloat defaultEmissive[] = {0.0, 0.0, 0.0, 1.0};
GLdouble bulletlife = 10.0;
GLfloat robotMovementSpeed = 0.04;

// ------------------------
// ###### ROBOT ARMY ######
// ------------------------

std::vector<RobotComponent> robotArmy;
std::vector<Robot_X_Z_Position> initial_robot_army_positions;

void initializeRobotArmy()
{
    robotArmy.clear();
    initial_robot_army_positions.clear();

    if(difficultyMultiplier == 1){
        for(size_t i = 0; i < 5; i++){

        }
    }
    if(difficultyMultiplier == 2){
        for(size_t i = 0; i < 7; i++){

        }
    }
    if(difficultyMultiplier == 3){
        for(size_t i = 0; i < 10; i++){

        }
    }

    RobotComponent robot1;
    robot1.robotStartingPosition.x = 50.0f;
    robot1.robotStartingPosition.y = 6.0f;
    robot1.robotStartingPosition.z = -40.0f;
    robotArmy.push_back(robot1);

    RobotComponent robot2;
    robot2.robotStartingPosition.x = 20.0f;
    robot2.robotStartingPosition.y = 6.0f;
    robot2.robotStartingPosition.z = -60.0f;
    robotArmy.push_back(robot2);

    RobotComponent robot3;
    robot3.robotStartingPosition.x = -10.0f;
    robot3.robotStartingPosition.y = 6.0f;
    robot3.robotStartingPosition.z = -70.0f;
    robotArmy.push_back(robot3);

    RobotComponent robot4;
    robot4.robotStartingPosition.x = -20.0f;
    robot4.robotStartingPosition.y = 6.0f;
    robot4.robotStartingPosition.z = -60.0f;
    robotArmy.push_back(robot4);

    RobotComponent robot5;
    robot5.robotStartingPosition.x = -30.0f;
    robot5.robotStartingPosition.y = 6.0f;
    robot5.robotStartingPosition.z = -35.0f;
    robotArmy.push_back(robot5);
}

// ------------------
// ###### MAIN ######
// ------------------

void initializeGameTimers()
{
    for (size_t i = 0; i < robotArmy.size(); ++i)
    {
        auto &robot = robotArmy[i];
        glutTimerFunc(16, updateRobotPosition, i);
        glutTimerFunc(16, animateRobot, i);
        glutTimerFunc(1000, fireBulletTimer, i);
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vHeight, vWidth);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Prototype terrain");

    initOpenGL(vHeight, vWidth);

    if (!loadOBJ("object.obj")) {
        printf("Error loading object file!\n");
        return -1;
    }

    initMouse();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardHandler);
    glutPassiveMotionFunc(mouseHandler); // Keep mouse motion handling
    glutMouseFunc(mouseButtonHandler);  // Add button handling

    initializeRobotArmy();
    initializeGameTimers();
    ballUpdateTimer(0); // Start ball updates

    glutMainLoop();
    return 0;
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void initLighting()
{
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};  // Low ambient light
    GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};  // Strong diffuse light
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f}; // White specular light
    GLfloat lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f}; // Directional light

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glEnable(GL_LIGHT0);   // Enable light 0
    glEnable(GL_LIGHTING); // Enable lighting in general
}

void initOpenGL(int w, int h)
{
    // Clear the framebuffer, set to black background R = 0, G = 0, B = 0
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (double)w / (double)h, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Initialize lighting and material shading.
    initLighting();

    // Ensure OpenGL uses the correct color material properties
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_TEXTURE_2D);

    // This is a 3D program. We need to enable depth testing.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Set up the camera
    if (isFreeLookEnabled) {
        float lookX = cos(cameraPitch) * sin(cameraYaw);
        float lookY = sin(cameraPitch);
        float lookZ = cos(cameraPitch) * cos(cameraYaw);
        gluLookAt(cameraX, cameraY, cameraZ,
                  cameraX + lookX, cameraY + lookY, cameraZ + lookZ,
                  0.0f, 1.0f, 0.0f);
    } else {
        gluLookAt(cameraX, cameraY, cameraZ,
                  0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0);
    }

    // Draw the scene
    drawGround();
    drawCannonRelativeToCamera();
    drawRobotMesh();
    drawBalls();

    glutSwapBuffers();
}

// ----------------------------
// ###### INPUT HANDLERS ######
// ----------------------------

void keyboardHandler(unsigned char key, int x, int y) {
    std::cout << "Key pressed: " << key << std::endl;
    switch (key) {
    case 'q':
    case 'Q':
    case 27:
        exit(0);
        break;
    case '1':
        robotArmy[0].stillalive = false;
        break;
    case '2':
        robotArmy[1].stillalive = false;
        break;
    case '3':
        robotArmy[2].stillalive = false;
        break;
    case '4':
        robotArmy[3].stillalive = false;
        break;
    case '5':
        robotArmy[4].stillalive = false;
        break;
    case '7': // Easy difficulty (5 robots)
        difficultyMultiplier = 1.0f;
        initializeRobotArmy(); // Reset robot positions
        std::cout << "Difficulty set to Easy" << std::endl;
        break;
    case '8': // Medium difficulty (7 robots)
        difficultyMultiplier = 2.0f;
        initializeRobotArmy(); // Reset robot positions
        std::cout << "Difficulty set to Medium" << std::endl;
        break;
    case '9': // Hard difficulty (10 robots)
        difficultyMultiplier = 3.0f;
        initializeRobotArmy(); // Reset robot positions
        std::cout << "Difficulty set to Hard" << std::endl;
        break;
    case 'R': // Restart the game
        initializeRobotArmy();
        break;
    }
    glutPostRedisplay();
}


void mouseHandler(int x, int y)
{
    static int centerX = vWidth / 2;
    static int centerY = vHeight / 2;
    static bool warpPointer = false;

    if (!warpPointer)
    {
        int deltaX = centerX - x; // Invert the X-axis movement
        int deltaY = centerY - y; // Invert the Y-axis movement if needed

        // Update camera yaw and pitch
        cameraYaw += deltaX * sensitivity * 0.01f;
        cameraPitch += deltaY * sensitivity * 0.01f;

        // Clamp pitch to prevent flipping
        if (cameraPitch > M_PI / 2.0f)
            cameraPitch = M_PI / 2.0f;
        if (cameraPitch < -M_PI / 2.0f)
            cameraPitch = -M_PI / 2.0f;

        // Re-center the mouse pointer
        warpPointer = true;
        glutWarpPointer(centerX, centerY);
    }
    else
    {
        warpPointer = false;
    }
    glutPostRedisplay(); // Redraw the scene
}

void initMouse()
{
    glutSetCursor(GLUT_CURSOR_NONE); // Hide the cursor
}

// -----------------------------------------------------------
// ###### OBJECT FILE HANDLERS & TEXTURES (.obj & .png) ######
// -----------------------------------------------------------

bool loadOBJ(const char *filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        printf("Failed to open OBJ file: %s\n", filePath);
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        { // Vertex
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (prefix == "f")
        { // Face
            Face f;
            iss >> f.v1 >> f.v2 >> f.v3;
            faces.push_back(f);
        }
    }
    return true;
}

GLuint loadTexture(const char *filePath)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("Failed to load texture: %s\n", filePath);
        return 0; // Return 0 to indicate failure
    }
    stbi_image_free(data);

    return textureID;
}

// -----------------------------------------
// ###### CAMERA UTILITY FUNCTIONS  ######
// -----------------------------------------

void getCameraForwardVector(float &fx, float &fy, float &fz)
{
    fx = cos(cameraPitch) * sin(cameraYaw);
    fy = sin(cameraPitch);
    fz = cos(cameraPitch) * cos(cameraYaw);
}

// -----------------------------------------
// ###### DRAWINGS  ######
// -----------------------------------------

void drawGround()
{
    GLuint textureID = loadTexture("texture/surface.png");
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, textureID);
    glColor3f(0.5f, 0.8f, 0.5f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-groundSize, 0.0f, -groundSize);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-groundSize, 0.0f, groundSize);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(groundSize, 0.0f, groundSize);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(groundSize, 0.0f, -groundSize);
    glEnd();
    glPopMatrix();
}

void fireCannonBall() {
    Ball newBall;

    // Offset values to position the ball at the tip of the cannon
    const float cannonOffsetX = 0.0f;  // Adjust as needed
    const float cannonOffsetY = -2.0f; // Slightly below the camera
    const float cannonOffsetZ = 1.5f;  // In front of the camera

    const float speedMultiplier = 5.0f;

    // Calculate the tip of the cannon in world space
    float offsetX = cos(cameraPitch) * sin(cameraYaw) * cannonOffsetZ;
    float offsetY = sin(cameraPitch) * cannonOffsetZ + cannonOffsetY;
    float offsetZ = cos(cameraPitch) * cos(cameraYaw) * cannonOffsetZ;

    newBall.x = cameraX + offsetX;
    newBall.y = cameraY + offsetY;
    newBall.z = cameraZ + offsetZ;

    // Direction vector based on the camera's orientation
    newBall.dx = cos(cameraPitch) * sin(cameraYaw) * speedMultiplier;
    newBall.dy = sin(cameraPitch) * speedMultiplier;
    newBall.dz = cos(cameraPitch) * cos(cameraYaw) * speedMultiplier;

    // Set ball lifespan
    newBall.lifespan = 15.0f; // Ball exists for 5 seconds

    // Add the ball to the active list
    activeBalls.push_back(newBall);
}

void mouseButtonHandler(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        fireCannonBall(); // Fire the ball when the left mouse button is pressed
    }
}

void drawBalls() {
    for (const auto &ball : activeBalls) {
        glPushMatrix();
        glTranslatef(ball.x, ball.y, ball.z);
        glutSolidSphere(0.4f, 16, 16); // Draw a sphere with radius 0.2
        glPopMatrix();
    }
}

void updateBalls(float deltaTime) {
    for (auto it = activeBalls.begin(); it != activeBalls.end();) {
        // Update position
        it->x += it->dx * deltaTime;
        it->y += it->dy * deltaTime;
        it->z += it->dz * deltaTime;

        // Decrease lifespan
        it->lifespan -= deltaTime;

        // Remove if expired
        if (it->lifespan <= 0) {
            it = activeBalls.erase(it);
        } else {
            ++it;
        }
    }
}

void ballUpdateTimer(int value) {
    updateBalls(0.16f); // Assume 60 FPS (deltaTime = 1/60)
    glutPostRedisplay();
    glutTimerFunc(16, ballUpdateTimer, 0);
}


void drawCannonRelativeToCamera()
{
    float yawAngle = -cameraYaw * (180.0f / M_PI);
    float pitchAngle = cameraPitch * (180.0f / M_PI);
    glPushMatrix();
    // Align Camera To On Top of the Cannon
    glTranslatef(cameraX, cameraY, cameraZ);
    glRotatef(-yawAngle, 0.0f, 1.0f, 0.0f);   // Rotate around the Y-axis
    glRotatef(-pitchAngle, 1.0f, 0.0f, 0.0f); // Rotate around the X-axis
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);       // Rotate 90 degrees around the X-axis
    glTranslatef(0.0f, -0.5f, 1.5f);          // Adjust as needed to position the cannon

    // Draw the cannon
    glColor3f(1.0f, 1.0f, 1.0f); // White object
    glBegin(GL_POLYGON);
    for (const Face &face : faces)
    {
        const Vertex &v1 = vertices[face.v1 - 1];
        const Vertex &v2 = vertices[face.v2 - 1];
        const Vertex &v3 = vertices[face.v3 - 1];
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
    }
    glEnd();
    glPopMatrix();
}

// -----------------------------------------
// ###### DRAWINGS ROBOT 1 ######
// -----------------------------------------

void drawRobotMesh(void)
{
    for (const auto &robot : robotArmy)
    {
        // Apply translation to move the robot forward along the z-axis
        glPushMatrix();
        glTranslatef(robot.robotStartingPosition.x, robot.robotStartingPosition.y, robot.robotStartingPosition.z);

        // Robot Core Body
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glScalef(1, 1, 1);
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        glColor3f(0.75, 0.75, 0.75);
        drawRobotChestMiddleFrustum(); // Middle Chest.

        glPushMatrix();
        glTranslatef(0.0, 2.0, 0.0);
        glScalef(1, 1, 1);
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        glColor3f(1.0, 0.0, 0.0);
        drawRobotChestUpperFrustum(); // Shoulders and Upper Chest.
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, -1.0, 0.0);
        glScalef(1, 1, 1);
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        drawRobotChestLowerFrustum(); // Lower Chest.
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0.0, -1.5, 0.0);
        glScalef(1, 1, 1);
        drawRobotChestBottomLegFrustum(); // Leg Division.
        glPopMatrix();
        glPopMatrix();

        // Robot Right Arm
        glPushMatrix();
        glTranslatef(2.4, 3.2, 0.0);
        glScalef(1, 1, 1);
        glTranslatef(2.4, -0.8, 0.0); // Rotation along a specific focal point.
        glRotatef(robot.spinRobotArmValue, 1.0, 0.0, 0.0);
        glTranslatef(-2.4, 0.8, 0.0);
        drawRobotArm();
        glPopMatrix();

        // Robot Left Arm
        glPushMatrix();
        glTranslatef(-2.4, 3.2, 0.0);
        glScalef(1, 1, 1);
        glRotatef(180.0, 0.0, 1.0, 0.0); // Rotating the arm by 180 deg on y-axis.
        glTranslatef(2.4, -0.8, 0.0);    // Rotation along a specific focal point.
        glRotatef(robot.spinRobotArmValue, 1.0, 0.0, 0.0);
        glTranslatef(-2.4, 0.8, 0.0);
        drawRobotArm();
        glPopMatrix();

        // Robot face
        glPushMatrix();
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        glTranslatef(0.0, 3.55, 0.0);
        glScalef(1, 1, 1);
        drawRobotFace();
        glPopMatrix();

        // Robot Cannon
        glPushMatrix();
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        glTranslatef(1.0, 5.0, 0.0);
        glScalef(1, 1, 1);

        if (robot.stillalive)
        {
            glRotatef(10.0f, 1.0, 0.0, 0.0); // Rotate around X-axis

            // draw bullets
            drawBulletPoint(robot.bullets);

            // animate cannon spinning
            int time = glutGet(GLUT_ELAPSED_TIME);
            float sidewaysAngle = 30.0f * sin(time / 1000.0f);
            glRotatef(sidewaysAngle, 0.0, 1.0, 0.0);              // Rotate along X-axis for sideways motion
            glRotatef(robot.spinRobotCannonValue, 0.0, 0.0, 1.0); // Rotating along Y axis.
            gluCylinder(robot.quadric, 0.35, 0.35, 3.5, 32, 32);  // Base radius, top radius, height, slices, stacks
        }
        else
        {
            glColor4f(0.5, 0.5, 0.5, 0.3);
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);   // Rotate around X-axis
            glRotatef(robot.spinRobotCannonValue, 0.0, 0.0, 1.0); // Rotating along Y axis.
            gluCylinder(robot.quadric, 0.35, 0.35, 3.5, 32, 32);  // Base radius, top radius, height, slices, stacks
        }
        glPushMatrix();
        glTranslatef(0.0, 0.34, 3.0);
        drawCannonSmallAimSupport();
        glPopMatrix();
        glPopMatrix();

        // Robot Cannon Supporter
        glPushMatrix();
        if (!robot.stillalive)
        {
            glRotatef(robot.afterDeadBodyAngle, 1.0, 0.0, 0.0);
        }
        glTranslatef(1.0, 3, 0.0);
        glScalef(1, 1, 1);
        drawRobotCannonSupport();
        glTranslatef(0.0, 2, 0.0);
        glutSolidSphere(0.5, 20, 20);
        glPopMatrix();

        // Right leg
        glPushMatrix();
        glTranslatef(0.8, -3.0, 0.0);
        glScalef(1, 1, 1);
        glTranslatef(0.0, 2.0, 0.0);
        glRotatef((robot.upperLegAngle * robot.alternateLegRotate), 1.0, 0.0, 0.0);
        glTranslatef(0.0, -2.0, 0.0);
        drawUpperLeg();
        glPushMatrix();
        glTranslatef(0.0, -3, 0);
        glScalef(1, 1, 1);
        glTranslatef(0.0, 3.0, 0.0);
        glRotatef(robot.lowerLegAngle, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -3.0, 0.0);
        drawLowerLeg();
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-0.8, -3.0, 0.0);
        glScalef(1, 1, 1);
        glTranslatef(0.0, 2.0, 0.0);
        glRotatef((robot.upperLegAngle * -1.0 * robot.alternateLegRotate), 1.0, 0.0, 0.0);
        glTranslatef(0.0, -2.0, 0.0);
        drawUpperLeg();
        glPushMatrix();
        glTranslatef(0.0, -3, 0);
        glScalef(1, 1, 1);
        glTranslatef(0.0, 3.0, 0.0);
        glRotatef(robot.lowerLegAngle, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -3.0, 0.0);
        drawLowerLeg();
        glPopMatrix();
        glPopMatrix();
        glPopMatrix();
    }
}

void drawBulletPoint(const std::vector<Bullet> &bullets)
{
    for (const auto &bullet : bullets)
    {
        glPushMatrix();
        glRotatef(bullet.sidewaysAngle, 0.0, 1.0, 0.0);
        glTranslatef(bullet.x, bullet.y, bullet.z);
        // Set material to emit red light
        glMaterialfv(GL_FRONT, GL_EMISSION, emissiveColor);

        // Draw the main bullet
        glutSolidSphere(0.3, 16, 16);

        // Reset material emission to default
        glMaterialfv(GL_FRONT, GL_EMISSION, defaultEmissive);

        // Draw a semi-transparent halo
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0, 0.0, 0.0, 0.3); // Semi-transparent red
        glutSolidSphere(0.15, 16, 16);
        glDisable(GL_BLEND);
        glPopMatrix();
    }
}

void animateRobot(int val)
{
    auto &robot = robotArmy[val];
    if (robot.stillalive)
    {
        if (robot.steppingstage == 0 && robot.upperLegAngle >= -30.0f)
        {
            robot.upperLegAngle -= 1.0f;
            glutPostRedisplay();
        }
        if (robot.steppingstage == 0 && robot.lowerLegAngle <= 30.0f)
        {
            robot.lowerLegAngle += 1.0;
            glutPostRedisplay();
        }
        if (robot.upperLegAngle < -30 && robot.lowerLegAngle > 30 && robot.steppingstage == 0)
        {
            robot.steppingstage = 1;
        }
        if (robot.steppingstage == 1 && robot.lowerLegAngle >= 10.0f)
        {
            robot.lowerLegAngle -= 1.0;
            glutPostRedisplay();
        }
        if (robot.steppingstage == 1 && robot.lowerLegAngle < 10.0f)
        {
            robot.steppingstage = 2;
        }
        if (robot.steppingstage == 2 && robot.upperLegAngle < 0.0f)
        {
            robot.upperLegAngle += 1.0;
            glutPostRedisplay();
        }
        if (robot.steppingstage == 2 && robot.lowerLegAngle > 0.0f)
        {
            robot.lowerLegAngle -= 1.0;
            glutPostRedisplay();
        }
        if (robot.steppingstage == 2 && robot.lowerLegAngle == 0.0f && robot.upperLegAngle == 0.0f)
        {
            robot.steppingstage = 0;
            robot.alternateLegRotate = robot.alternateLegRotate * -1.0;
        }

        // Robot Arm Rotation
        float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        robot.spinRobotArmValue = sin(currentTime * speedFactor) * maxAngle;

        // Cannon Rotation
        robot.spinRobotCannonValue += 2;
        if (robot.spinRobotCannonValue > 360.0)
            robot.spinRobotCannonValue -= 360.0;

        updateBullets(robot.bullets, 0.07f);
        glutPostRedisplay();
    }
    else
    {
        if (robot.afterDeadBodyAngle < 31)
        {
            robot.afterDeadBodyAngle += 1;
        }
    }
    glutTimerFunc(16, animateRobot, val);
}

void updateBullets(std::vector<Bullet> &bullets, float deltaTime)
{
    for (auto it = bullets.begin(); it != bullets.end();)
    {
        // Update position based on direction and time
        it->x += it->dx * deltaTime;
        it->y += it->dy * deltaTime;
        it->z += it->dz * deltaTime;

        // Decrease lifespan
        it->lifespan -= deltaTime;

        // Remove the bullet if its lifespan ends
        if (it->lifespan <= 0)
        {
            it = bullets.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void fireBullet(std::vector<Bullet> &bullets, float cannonX, float cannonY, float cannonZ, float cannonDX, float cannonDY, float cannonDZ, float sidewaysAngle)
{
    // Create a bullet and append it in the bullets list
    Bullet newBullet;
    newBullet.x = cannonX;
    newBullet.y = cannonY;
    newBullet.z = cannonZ;
    newBullet.dx = cannonDX;
    newBullet.dy = cannonDY;
    newBullet.dz = cannonDZ;
    newBullet.lifespan = 10.0f; // 10 seconds lifespan
    newBullet.sidewaysAngle = sidewaysAngle;
    bullets.push_back(newBullet);
}

void fireBulletTimer(int value)
{
    auto &robot = robotArmy[value];
    if (robot.stillalive)
    {
        int time = glutGet(GLUT_ELAPSED_TIME);
        float sidewaysAngle = 30.0f * sin(time / 1000.0f);
        fireBullet(robot.bullets, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0, sidewaysAngle);
    }
    glutTimerFunc(1000, fireBulletTimer, value);
}

void updateRobotPosition(int value) {
    if(allRobotsDeadStatus()){
        robotArmy.clear();
        initializeRobotArmy();
        std::cout << "All robots are dead. Resetting robot army." << std::endl;
    }
    auto &robot = robotArmy[value];
    if (robot.stillalive) {
        robot.robotStartingPosition.z += robotMovementSpeed * difficultyMultiplier;
        if (robot.robotStartingPosition.z > 25) {
            robot.stillalive = false;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(16, updateRobotPosition, value);
}

bool allRobotsDeadStatus(){
    for (size_t i = 0; i < robotArmy.size(); ++i)
    {
        auto &robot = robotArmy[i];
        if (robot.stillalive) {
            return false;
        }
    }
    return true;
}