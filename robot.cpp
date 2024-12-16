#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include "robot.h"
#include <vector>

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void keyboardHandler(unsigned char key, int x, int y);
void specialFunctionHandler(int key, int x, int y);
void spinRobotArm(float rotationValue);
void spinRobotCannon(void);
void deactivateKeyboardHandlers(void);
void rotateQuad(float val);
void rotateShin(float val);
void animateRobot(int val);
void drawBullets(void);
void fireBullet(float cannonX, float cannonY, float cannonZ, float cannonDX, float cannonDY, float cannonDZ);
void updateBullets(float deltatime);
void drawBulletPoint(void);
void fireBulletTimer(int timer);

/* Window Settings */
GLint windowWidth = 900;
GLint windowHeight = 900;
GLint viewportHeight = windowHeight;
GLint viewportWidth = windowWidth;
GLdouble worldLeft = -800.0;
GLdouble worldRight = 800.0;
GLdouble worldBottom = -800.0;
GLdouble worldTop = 800.0;
GLdouble spinRobotArmValue = 0.0;
GLdouble speedFactor = 2.4f; // Speed for 62.5 Hz oscillation
GLdouble maxAngle = 35.0f;
GLdouble spinRobotCannonValue = 0.0;

std::vector<Bullet> bullets;                    // Container to store active bullets
GLfloat emissiveColor[] = {1.0, 0.0, 0.0, 1.0}; // Red emissive light
GLfloat haloColor[] = {1.0, 0.0, 0.0, 0.3};     // Transparent red halo
GLfloat defaultEmissive[] = {0.0, 0.0, 0.0, 1.0};
GLdouble bulletlife = 10.0; // sec

/* Quadric surfaces initial pointers */
GLUquadric *quadric = nullptr;
GLint steppingstage = 0; // 0, 1, 2, 3
float upperLegAngle = 0.0f;
float lowerLegAngle = 0.0f;
float alternateLegRotate = 1.0f;

/* Camera Settings */
double cameraFront[] = {0.0, 5.0, 15.0}; // Looking at origin from the +ve z-axis
double cameraBack[] = {0.0, 5.0, -15.0}; // Looking at origin from the -ve z-axis
double cameraRight[] = {15.0, 5.0, 0.0}; // Looking at origin from the +ve z-axis
double cameraLeft[] = {-15.0, 5.0, 0.0}; // Looking at origin from the -ve z-axis
double cameraFrontTopRight[] = {10.0, 10.0, 10.0};
double cameraFrontTopLeft[] = {-10.0, 10.0, 10.0};
double cameraBackTopLeft[] = {-10.0, 10.0, -10.0};
double cameraBackTopRight[] = {10.0, 10.0, -10.0};
double cameraFrontBottomRight[] = {10.0, -10.0, 10.0};
double cameraBackButtonRight[] = {10.0, -10.0, -10.0};
double cameraFrontBottomLeft[] = {-10.0, -10.0, 10.0};
double cameraBackButtonLeft[] = {-10.0, -10.0, -10.0};
double cameraPosition[] = {cameraFrontBottomRight[0], cameraFrontBottomRight[1], cameraFrontBottomRight[2]};

void updateCameraPosition(const double newPosition[3])
{
  cameraPosition[0] = newPosition[0];
  cameraPosition[1] = newPosition[1];
  cameraPosition[2] = newPosition[2];
}

/* Keyboard key activations */
bool cameraKeyPressed = false; // 'v' or 'V'
bool handsKeyPressed = false;  // 'h' or 'H'
bool cannonKeyPressed = true;  // 'c' or 'C'
bool upperLegkey = false;
bool lowerLegKey = false;
bool isStepping = true;
/* Main function */
int main(int argc, char **argv)
{
  glutInit(&argc, (char **)argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(windowHeight, windowWidth);
  glutInitWindowPosition(200, 200);
  glutCreateWindow("Assignment1 Robot 3D Render");

  initOpenGL(windowHeight, windowWidth);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboardHandler);
  glutSpecialFunc(specialFunctionHandler);

  glutTimerFunc(16, animateRobot, 0);
  glutTimerFunc(4000, fireBulletTimer, 0);
  glutMainLoop();
  return 0;
}

/* Lighting */
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

/* Initialize the system */
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

  // This is a 3D program. We need to enable depth testing.
  glEnable(GL_DEPTH_TEST);
}

/* Robot Model */
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  // Positioning the camera (i.e. setting up the Viewing Transformation (V matrix)).
  gluLookAt(
      cameraPosition[0], cameraPosition[1], cameraPosition[2], // Eye position (camera position).
      0.0, 0.0, 0.0,                                           // Center position (point the camera is looking at).
      0.0, 1.0, 0.0                                            // Up vector (defines which direction is "up" for the camera).
  );

  // Robot Core Body
  glPushMatrix();
  glTranslatef(0.0, 0.0, 0.0);
  glScalef(1, 1, 1);
  glColor3f(0.75, 0.75, 0.75);
  drawRobotChestMiddleFrustum(); // Middle Chest. 

  glPushMatrix();
  glTranslatef(0.0, 2.0, 0.0);
  glScalef(1, 1, 1);
  glColor3f(1.0, 0.0, 0.0);
  drawRobotChestUpperFrustum(); // Shoulders and Upper Chest.
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0, -1.0, 0.0);
  glScalef(1, 1, 1);
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
  glRotatef(spinRobotArmValue, 1.0, 0.0, 0.0);
  glTranslatef(-2.4, 0.8, 0.0);
  drawRobotArm();
  glPopMatrix();

  // Robot Left Arm
  glPushMatrix();
  glTranslatef(-2.4, 3.2, 0.0);
  glScalef(1, 1, 1);
  glRotatef(180.0, 0.0, 1.0, 0.0); // Rotating the arm by 180 deg on y-axis.
  glTranslatef(2.4, -0.8, 0.0);    // Rotation along a specific focal point.
  glRotatef(spinRobotArmValue, 1.0, 0.0, 0.0);
  glTranslatef(-2.4, 0.8, 0.0);
  drawRobotArm();
  glPopMatrix();

  // Robot face
  glPushMatrix();
  glTranslatef(0.0, 3.55, 0.0);
  glScalef(1, 1, 1);
  drawRobotFace();
  glPopMatrix();

  // Robot Cannon
  glPushMatrix();
  if (!quadric)
  {
    quadric = gluNewQuadric();
  }
  glTranslatef(1.0, 5.0, 0.0);
  glScalef(1, 1, 1);

  int time = glutGet(GLUT_ELAPSED_TIME);
  float sidewaysAngle = 30.0f * sin(time / 1000.0f);

  glRotatef(sidewaysAngle, 0.0, 1.0, 0.0);        // Rotate along X-axis for sideways motion
  glRotatef(5.0f, 1.0, 0.0, 0.0);                 // Rotate around X-axis
  glRotatef(spinRobotCannonValue, 0.0, 0.0, 1.0); // Rotating along Y axis.
  gluCylinder(quadric, 0.35, 0.35, 3.5, 32, 32);  // Base radius, top radius, height, slices, stacks
  glPushMatrix();
  glTranslatef(0.0, 0.34, 3.0);
  drawCannonSmallAimSupport();
  glPopMatrix();
  // drawing the bullets
  drawBulletPoint();
  glPopMatrix();

  // Robot Cannon Supporter
  glPushMatrix();
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
  glRotatef((upperLegAngle * alternateLegRotate), 1.0, 0.0, 0.0);
  glTranslatef(0.0, -2.0, 0.0);
  drawUpperLeg();
  glPushMatrix();
  glTranslatef(0.0, -3, 0);
  glScalef(1, 1, 1);
  glTranslatef(0.0, 3.0, 0.0);
  glRotatef(lowerLegAngle, 1.0, 0.0, 0.0);
  glTranslatef(0.0, -3.0, 0.0);
  drawLowerLeg();
  glPopMatrix();
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-0.8, -3.0, 0.0);
  glScalef(1, 1, 1);
  glTranslatef(0.0, 2.0, 0.0);
  glRotatef((upperLegAngle * -1.0 * alternateLegRotate), 1.0, 0.0, 0.0);
  glTranslatef(0.0, -2.0, 0.0);
  drawUpperLeg();
  glPushMatrix();
  glTranslatef(0.0, -3, 0);
  glScalef(1, 1, 1);
  glTranslatef(0.0, 3.0, 0.0);
  glRotatef(lowerLegAngle, 1.0, 0.0, 0.0);
  glTranslatef(0.0, -3.0, 0.0);
  drawLowerLeg();
  glPopMatrix();
  glPopMatrix();
  glutSwapBuffers();
}

void drawBulletPoint()
{
  for (const auto &bullet : bullets)
  {
    glPushMatrix();
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

/* Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (double)w / (double)h, 0.1, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

/* Keyboard Handlers */
void keyboardHandler(unsigned char key, int x, int y)
{
  if (key == 'V' || key == 'v')
  {
    cameraKeyPressed = true;
  }
  else if (cameraKeyPressed)
  {
    switch (key)
    {
    case '1':
      updateCameraPosition(cameraFront);
      break;
    case '2':
      updateCameraPosition(cameraBack);
      break;
    case '3':
      updateCameraPosition(cameraFrontTopRight);
      break;
    case '4':
      updateCameraPosition(cameraFrontTopLeft);
      break;
    case '5':
      updateCameraPosition(cameraBackTopLeft);
      break;
    case '6':
      updateCameraPosition(cameraBackTopRight);
      break;
    case '7':
      updateCameraPosition(cameraFrontBottomRight);
      break;
    case '8':
      updateCameraPosition(cameraFrontBottomLeft);
      break;
    case '9':
      updateCameraPosition(cameraBackButtonLeft);
      break;
    case '0':
      updateCameraPosition(cameraBackButtonRight);
      break;
    case 'L':
    case 'l':
      updateCameraPosition(cameraLeft);
      break;
    case 'R':
    case 'r':
      updateCameraPosition(cameraRight);
      break;
    default:
      cameraKeyPressed = false;
      break;
    }
    cameraKeyPressed = false;
  }

  switch (key)
  {
  case 'B':
  case 'b':
    fireBullet(0.0, 0.0, 0.0, 0.0, 0.0, 10.0);
    break;
  default:
    break;
  }
  glutPostRedisplay(); // Redisplay the scene
}

void specialFunctionHandler(int key, int x, int y)
{
  if (handsKeyPressed)
  {
    switch (key)
    {
    case GLUT_KEY_UP:
      spinRobotArm(3.0);
      break;
    case GLUT_KEY_DOWN:
      spinRobotArm(-3.0);
      break;
    default:
      break;
    }
  }
  if (lowerLegKey)
  {
    switch (key)
    {
    case GLUT_KEY_UP:
      rotateShin(0.5);
      break;
    case GLUT_KEY_DOWN:
      rotateShin(-0.5);
      break;
    default:
      break;
    }
  }
  if (upperLegkey)
  {
    switch (key)
    {
    case GLUT_KEY_UP:
      rotateQuad(1.0);
      break;
    case GLUT_KEY_DOWN:
      rotateQuad(-1.0);
      break;
    default:
      break;
    }
  }

  glutPostRedisplay(); // Redisplay the scene;
}

/* Spin Activations */
void spinRobotArm(float rotationValue)
{
  spinRobotArmValue += rotationValue;
  if (spinRobotArmValue > 360.0)
    spinRobotArmValue -= 360.0;
}

void rotateQuad(float val)
{
  // Increment or decrement the angles to simulate stepping motion
  if (upperLegAngle <= -30.f)
  {
    // Reset the angle after a certain point
    upperLegAngle = -30.0f;
  }
  else if (upperLegAngle >= 30.f)
  {
    upperLegAngle = 30.0f;
  }

  upperLegAngle += val;

  // Call this function again after 16 milliseconds (~60 frames per second)
}

void rotateShin(float val)
{
  // Increment or decrement the angles to simulate stepping motion
  if (lowerLegAngle >= 30.0f)
  {
    // Reset the angle after a certain point
    lowerLegAngle = 30.0f;
  }
  else if (lowerLegAngle <= 0.0f)
  {
    lowerLegAngle = 0.0f;
  }
  lowerLegAngle += val;

  // Call this function again after 16 milliseconds (~60 frames per second)
}

void animateRobot(int val)
{
  if (!isStepping)
  {
    steppingstage = 0;
    return;
  }
  if (steppingstage == 0 && upperLegAngle >= -30.0f)
  {
    upperLegAngle -= 1.0f;
    glutPostRedisplay();
  }
  if (steppingstage == 0 && lowerLegAngle <= 30.0f)
  {
    lowerLegAngle += 1.0;
    glutPostRedisplay();
  }
  if (upperLegAngle < -30 && lowerLegAngle > 30 && steppingstage == 0)
  {
    steppingstage = 1;
  }
  if (steppingstage == 1 && lowerLegAngle >= 10.0f)
  {
    lowerLegAngle -= 1.0;
    glutPostRedisplay();
  }
  if (steppingstage == 1 && lowerLegAngle < 10.0f)
  {
    steppingstage = 2;
  }
  if (steppingstage == 2 && upperLegAngle < 0.0f)
  {
    upperLegAngle += 1.0;
    glutPostRedisplay();
  }
  if (steppingstage == 2 && lowerLegAngle > 0.0f)
  {
    lowerLegAngle -= 1.0;
    glutPostRedisplay();
  }
  if (steppingstage == 2 && lowerLegAngle == 0.0f && upperLegAngle == 0.0f)
  {
    steppingstage = 0;
    alternateLegRotate = alternateLegRotate * -1.0;
  }
  if (isStepping)
  {
    // Robot Arm Rotation
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    spinRobotArmValue = sin(currentTime * speedFactor) * maxAngle;

    // Cannon Rotation
    spinRobotCannonValue += 2;
    if (spinRobotCannonValue > 360.0)
      spinRobotCannonValue -= 360.0;

    updateBullets(0.07f);
    glutPostRedisplay();
    glutTimerFunc(16, animateRobot, 0);
  }
}

void updateBullets(float deltaTime)
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

void fireBullet(float cannonX, float cannonY, float cannonZ, float cannonDX, float cannonDY, float cannonDZ)
{
  // Create a bullet and append it in the bullets list
  Bullet newBullet;
  newBullet.x = cannonX;
  newBullet.y = cannonY;
  newBullet.z = cannonZ;
  newBullet.dx = cannonDX;
  newBullet.dy = cannonDY;
  newBullet.dz = cannonDZ;
  newBullet.lifespan = 10.0f; // 2 seconds lifespan
  bullets.push_back(newBullet);
}

void fireBulletTimer(int value)
{
  fireBullet(0.0, 0.0, 0.0, 0.0, 0.0, 10.0);
  glutTimerFunc(1000, fireBulletTimer, 0);
}
