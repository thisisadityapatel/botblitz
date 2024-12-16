#define GL_SILENCE_DEPRECATION
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

struct Bullet {
    float x, y, z;        
    float dx, dy, dz;      
    float lifespan;
    float sidewaysAngle;
};

struct RobotInitialPosition
{
    float x, y, z;
};

/* Common robot component dimensions */
float robotWaistWidth = 1;

/* Function to draw a frustum between two sets of vertices */
void drawFrustum(GLfloat lowerBaseVertices[4][3], GLfloat upperBaseVertices[4][3]) {
    // Drawing bottom face
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f);  // Normal pointing downward
        glVertex3fv(lowerBaseVertices[0]);
        glVertex3fv(lowerBaseVertices[1]);
        glVertex3fv(lowerBaseVertices[2]);
        glVertex3fv(lowerBaseVertices[3]);
    glEnd();

    // Drawing top face
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);  // Normal pointing upward
        glVertex3fv(upperBaseVertices[0]);
        glVertex3fv(upperBaseVertices[1]);
        glVertex3fv(upperBaseVertices[2]);
        glVertex3fv(upperBaseVertices[3]);
    glEnd();

    // Drawing sides (connecting the lower and upper faces)
    glBegin(GL_QUADS);
        // Side 1
        glNormal3f(0.0f, 0.0f, -1.0f);  // Normal for side 1
        glVertex3fv(lowerBaseVertices[0]);
        glVertex3fv(lowerBaseVertices[1]);
        glVertex3fv(upperBaseVertices[1]);
        glVertex3fv(upperBaseVertices[0]);

        // Side 2
        glNormal3f(1.0f, 0.0f, 0.0f);  // Normal for side 2
        glVertex3fv(lowerBaseVertices[1]);
        glVertex3fv(lowerBaseVertices[2]);
        glVertex3fv(upperBaseVertices[2]);
        glVertex3fv(upperBaseVertices[1]);

        // Side 3
        glNormal3f(0.0f, 0.0f, 1.0f);  // Normal for side 3
        glVertex3fv(lowerBaseVertices[2]);
        glVertex3fv(lowerBaseVertices[3]);
        glVertex3fv(upperBaseVertices[3]);
        glVertex3fv(upperBaseVertices[2]);

        // Side 4
        glNormal3f(-1.0f, 0.0f, 0.0f);  // Normal for side 4
        glVertex3fv(lowerBaseVertices[3]);
        glVertex3fv(lowerBaseVertices[0]);
        glVertex3fv(upperBaseVertices[0]);
        glVertex3fv(upperBaseVertices[3]);
    glEnd();
}

/* Specific function to draw the upper chest frustum */
void drawRobotChestUpperFrustum() {
    GLfloat lowerBaseVertices[4][3] = {
        {-1.8f, 0.0f, -(robotWaistWidth/2)},
        { 1.8f, 0.0f, -(robotWaistWidth/2)},
        { 1.8f, 0.0f, (robotWaistWidth/2)},
        {-1.8f, 0.0f, (robotWaistWidth/2)}
    };

    GLfloat upperBaseVertices[4][3] = {
        {-1.8f, 0.0f + robotWaistWidth, -(robotWaistWidth/2)},
        { 1.8f, 0.0f + robotWaistWidth, -(robotWaistWidth/2)},
        { 1.8f, 0.0f + robotWaistWidth, (robotWaistWidth/2)},
        {-1.8f, 0.0f + robotWaistWidth, (robotWaistWidth/2)}
    };

    drawFrustum(lowerBaseVertices, upperBaseVertices);
}

/* Specific function to draw the middle chest frustum */
void drawRobotChestMiddleFrustum() {
    GLfloat lowerBaseVertices[4][3] = {
        {-1.2f, 0.0f, -(robotWaistWidth/2)},
        { 1.2f, 0.0f, -(robotWaistWidth/2)},
        { 1.2f, 0.0f, (robotWaistWidth/2)},
        {-1.2f, 0.0f, (robotWaistWidth/2)}
    };

    GLfloat upperBaseVertices[4][3] = {
        {-1.8f, 2.0f, -(robotWaistWidth/2)},
        { 1.8f, 2.0f, -(robotWaistWidth/2)},
        { 1.8f, 2.0f, (robotWaistWidth/2)},
        {-1.8f, 2.0f, (robotWaistWidth/2)}
    };

    drawFrustum(lowerBaseVertices, upperBaseVertices);
}

/* Specific function to draw the lower chest frustum */
void drawRobotChestLowerFrustum() {
    GLfloat lowerBaseVertices[4][3] = {
        {-1.4f, 0.0f, -(robotWaistWidth/2)},
        { 1.4f, 0.0f, -(robotWaistWidth/2)},
        { 1.4f, 0.0f, (robotWaistWidth/2)},
        {-1.4f, 0.0f, (robotWaistWidth/2)}
    };

    GLfloat upperBaseVertices[4][3] = {
        {-1.2f, 1.0f, -(robotWaistWidth/2)},
        { 1.2f, 1.0f, -(robotWaistWidth/2)},
        { 1.2f, 1.0f, (robotWaistWidth/2)},
        {-1.2f, 1.0f, (robotWaistWidth/2)}
    };

    drawFrustum(lowerBaseVertices, upperBaseVertices);
}

void drawRobotChestBottomLegFrustum() {
    GLfloat lowerBaseVertices[4][3] = {
        {-0.2f, 0.0f, -(robotWaistWidth/2)},
        { 0.2f, 0.0f, -(robotWaistWidth/2)},
        { 0.2f, 0.0f, (robotWaistWidth/2)},
        {-0.2f, 0.0f, (robotWaistWidth/2)}
    };

    GLfloat upperBaseVertices[4][3] = {
        {-0.2f, 0.5f, -(robotWaistWidth/2)},
        { 0.2f, 0.5f, -(robotWaistWidth/2)},
        { 0.2f, 0.5f, (robotWaistWidth/2)},
        {-0.2f, 0.5f, (robotWaistWidth/2)}
    };

    drawFrustum(lowerBaseVertices, upperBaseVertices);
}

void drawRobotArm(){
    GLfloat lowerBaseUpperArm[4][3] = {
        {-0.6f, -2.2f, -0.6f},
        { 0.6f, -2.2f, -0.6f},
        { 0.6f, -2.2f, 0.6f},
        {-0.6f, -2.2f, 0.6f}
    };

    GLfloat upperBaseUpperArm[4][3] = {
        {-0.6f, 0.0f, -0.6f},
        { 0.6f, 0.0f, -0.6f},
        { 0.6f, 0.0f, 0.6f},
        {-0.6f, 0.0f, 0.6f}
    };
    GLfloat lowerBaseLowerArm[4][3] = {
        {-0.5f, -4.5f, -0.5f},
        { 0.5f, -4.5f, -0.5f},
        { 0.5f, -4.5f, 0.5f},
        {-0.5f, -4.5f, 0.5f}
    };

    GLfloat upperBaseLowerArm[4][3] = {
        {-0.35f, -2.2f, -0.35f},
        { 0.35f, -2.2f, -0.35f},
        { 0.35f, -2.2f, 0.35f},
        {-0.35f, -2.2f, 0.35f}
    };

    GLfloat lowerBaseWrist[4][3] = {
        {-0.0f, -5.0f, -0.3f},
        { 0.3f, -5.0f, -0.3f},
        { 0.3f, -5.0f, 0.3f},
        {-0.0f, -5.0f, 0.3f}
    };

    GLfloat upperBaseWrist[4][3] = {
        {-0.0f, -4.5f, -0.3f},
        { 0.3f, -4.5f, -0.3f},
        { 0.3f, -4.5f, 0.3f},
        {-0.0f, -4.5f, 0.3f}
    };

    GLfloat lowerBaseWristFringer[4][3] = {
        {-0.3f, -5.0f, -0.3f},
        { 0.0f, -5.0f, -0.3f},
        { 0.0f, -5.0f, 0.3f},
        {-0.3f, -5.0f, 0.3f}
    };

    GLfloat upperBaseWristFingers[4][3] = {
        {-0.3f, -4.7f, -0.3f},
        { 0.0f, -4.7f, -0.3f},
        { 0.0f, -4.7f, 0.3f},
        {-0.3f, -4.7f, 0.3f}
    };
    glColor3f(0.0, 0.0, 1.0);  
    drawFrustum(lowerBaseUpperArm, upperBaseUpperArm);
    glColor3f(1.0, 0.0, 0.0); 
    drawFrustum(lowerBaseLowerArm, upperBaseLowerArm);
    glColor3f(0.75, 0.75, 0.75); 
    drawFrustum(lowerBaseWrist, upperBaseWrist);
    drawFrustum(lowerBaseWristFringer, upperBaseWristFingers);
}

void drawRobotFace(){
    GLfloat lowerBaseEyes[4][3] = {
        {-0.4f, 0.0f, -0.4f},
        { 0.4f, 0.0f, -0.4f},
        { 0.4f, 0.0f, 0.4f},
        {-0.4f, 0.0f, 0.4f}
    };

    GLfloat upperBaseEyes[4][3] = {
        {-0.4f, 0.5f, -0.6f},
        { 0.4f, 0.5f, -0.6f},
        { 0.4f, 0.5f, 0.6f},
        {-0.4f, 0.5f, 0.6f}
    };

    GLfloat lowerBaseMouth[4][3] = {
        {-0.4f, 0.0f, -0.4f},
        { 0.4f, 0.0f, -0.4f},
        { 0.4f, 0.0f, 0.4f},
        {-0.4f, 0.0f, 0.4f}
    };

    GLfloat upperBaseMouth[4][3] = {
        {-0.4f, -0.5f, -0.6f},
        { 0.4f, -0.5f, -0.6f},
        { 0.4f, -0.5f, 0.6f},
        {-0.4f, -0.5f, 0.6f}
    };
    
    drawFrustum(lowerBaseMouth, upperBaseMouth);
    glColor3f(0.0, 0.0, 1.0);  
    drawFrustum(lowerBaseEyes, upperBaseEyes);
    
    GLfloat lowerBaseLeftEar[4][3] = {
        {-0.6f, 0.7f, -0.4f},
        {-0.4f, 0.7f, -0.4f},
        {-0.4f, 0.7f, 0.4f},
        {-0.6f, 0.7f, 0.4f}
    };

    GLfloat upperBaseLeftEar[4][3] = {
        {-0.6f, -0.5f, -0.4f},
        {-0.4f, -0.5f, -0.4f},
        {-0.4f, -0.5f, 0.4f},
        {-0.6f, -0.5f, 0.4f}
    };
    
    drawFrustum(lowerBaseLeftEar, upperBaseLeftEar);

    GLfloat lowerBaseRightEar[4][3] = {
        {0.6f, 0.7f, -0.4f},
        {0.4f, 0.7f, -0.4f},
        {0.4f, 0.7f, 0.4f},
        {0.6f, 0.7f, 0.4f}
    };

    GLfloat upperBaseRightEar[4][3] = {
        {0.6f, -0.5f, -0.4f},
        {0.4f, -0.5f, -0.4f},
        {0.4f, -0.5f, 0.4f},
        {0.6f, -0.5f, 0.4f}
    };

    drawFrustum(lowerBaseRightEar, upperBaseRightEar);
}

void drawRobotCannonSupport(){
    GLfloat lowerBaseCannonSuppport[4][3] = {
        {-0.2f, 0.0f, -0.2},
        { 0.2f, 0.0f, -0.2},
        { 0.2f, 0.0f, 0.2},
        {-0.2f, 0.0f, 0.2}
    };

    GLfloat upperBaseCannonSuppport[4][3] = {
        {-0.2f, 2.0f, -0.2},
        { 0.2f, 2.0f, -0.2},
        { 0.2f, 2.0f, 0.2},
        {-0.2f, 2.0f, 0.2}
    };

    drawFrustum(lowerBaseCannonSuppport, upperBaseCannonSuppport);}

void drawUpperLeg(){
        glPushMatrix();
        glColor3f(1.0, 0, 0);
        glTranslatef(0.0f, 2.0f, 0.0f);
        glScalef(1.2f, 0.8f, 0.8f);  // Move up to the top of the upper leg
        glutSolidSphere(0.5f, 20, 20);  // Draw a sphere with radius 0.3
        glPopMatrix();
        GLfloat lowerBaseVertices[4][3] = {
        {-0.5f, 0.0f, -0.5f},
        { 0.5f, 0.0f, -0.5f},  
        { 0.5f, 0.0f, 0.5f},  
        {-0.5f, 0.0f, 0.5f} 
        };

        // Define the upper base of the upper leg (closer to the knee joint)
        GLfloat upperBaseVertices[4][3] = {
            {-0.5f, 2.0f, -0.5f},
            { 0.5f, 2.0f, -0.5f},
            { 0.5f, 2.0f, 0.5f},
            {-0.5f, 2.0f, 0.5f}
        };
        glColor3f(0.75, 0.75, 0.75);
        drawFrustum(lowerBaseVertices, upperBaseVertices);
}

void drawLowerLeg() {
        GLfloat lowerBaseVertices[4][3] = {
        {-0.55f, 0.0f, -0.55f},  
        { 0.55f, 0.0f, -0.55f},  
        { 0.55f, 0.0f, 0.55f},   
        {-0.55f, 0.0f, 0.55f}    
        };
        GLfloat upperBaseVertices[4][3] = {
            {-0.6f, 3.0f, -0.6f},  
            { 0.6f, 3.0f, -0.6f},  
            { 0.6f, 3.0f, 0.6f},   
            {-0.6f, 3.0f, 0.6f} 
        };

        GLfloat feetLowerBaseVertices[4][3] = {
        {-0.6f, 0.0f, -0.6f},  
        { 0.6f, 0.0f, -0.6f},  
        { 0.6f, 0.0f, 0.6f},   
        {-0.6f, 0.0f, 0.6f}    
        };

        GLfloat feetUpperBaseVertices[4][3] = {
            {-0.6f, 1.0f, -0.6f},  
            { 0.6f, 1.0f, -0.6f},  
            { 0.6f, 1.0f, -0.6f},   
            {-0.6f, 1.0f, -0.6f} 
        };

        glColor3f(0, 0, 1.0);
        glPushMatrix();
        glTranslatef(0.0f, 3.0f, 0.0f);
        glScalef(1.2f, 0.8f, 0.8f); 
        glutSolidSphere(0.5f, 20, 20);
        glPopMatrix();

        drawFrustum(lowerBaseVertices, upperBaseVertices);
        
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, 1.0f);
        drawFrustum(feetLowerBaseVertices, feetUpperBaseVertices);
        glPopMatrix();
}

void drawCannonSmallAimSupport(){
    GLfloat lowerCannonAim[4][3] = {
        {-0.1f, 0.0f, -0.2},
        { 0.1f, 0.0f, -0.2},
        { 0.1f, 0.0f, 0.2},
        {-0.1f, 0.0f, 0.2}
    };

    GLfloat upperCannonAim[4][3] = {
        {-0.1f, 0.5f, -0.1},
        { 0.1f, 0.5f, -0.1},
        { 0.1f, 0.5f, 0.1},
        {-0.1f, 0.5f, 0.1}
    };

    drawFrustum(lowerCannonAim, upperCannonAim);
}