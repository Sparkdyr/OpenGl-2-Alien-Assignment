#include <cmath>
#include <GL/freeglut.h>
#include "loadTGA.h"
#define _USE_MATH_DEFINES
#include <iostream>
#include <list>
#include <climits>
using namespace std;

float angle=0., look_x=0., look_z=0., eye_x=0., eye_z=5.;	//View parameters
float turnDirection = 0;
float moveDirection = 0;

int anim_wheel = 0;
int anim_hand = 0;

float alien_move = 0;
float alien_turn = 0;
float board = 0;
float alien_height = 0;
float alien_rotate = 0;

int value2 = 0;
int check = 1;
int grounded = 0;

float emission[4] = {0.3, 0.3, 0.3, 1.0};
float no_emission[4] = {0.0, 0.0, 0.0, 1.0};
float green[4] = {0, 1, 1, 1};

float light[4] = {15., 50., -20., 1.};  //light's position
float shadowMat[16] = {light[1],0,0,0, -light[0], 0, -light[2],-1,0,0, light[1],0, 0,0,0, light[1]};

struct particle	{
	int t;			//Life time  (0 - 200)
	float col;		//Color  (0 - 1)
	float size;		//Size   (5 - 25)
	float pos[3];	
	float vel[3];
};
list<particle> parList;
int tick = 0;


GLuint txId[9];
#define GL_CLAMP_TO_EDGE 0x812F

void newParticle()
{
particle p = {0};
p.pos[0] = 0.5 * (rand() / (float)RAND_MAX) - 0.25;
p.pos[1] = -0.7;
p.pos[2] = 0.5 * (rand() / (float)RAND_MAX - 0.25);

p.vel[0] = -0.01 * (rand() / (float)RAND_MAX) - 0.005;
p.vel[1] = -0.3;
p.vel[2] = -0.01 * (rand() / (float)RAND_MAX) - 0.055;

p.size = 1;
p.col = 1;
parList.push_back(p);
}

void drawParticle(float col, float size, float px, float py, float pz) {
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glColor3f(col, col, col);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, txId[8]);

	glPushMatrix();
		glTranslatef(px/3, py/3, pz/3);
		glScalef(size/3, size/3, size/3);

		glBegin(GL_QUADS);
			//A quad on the xy-plane
			glTexCoord2f(0, 0);
			glVertex3f(-0.5, -0.5, 0);
			glTexCoord2f(1, 0);
			glVertex3f(0.5, -0.5, 0);
			glTexCoord2f(1, 1);
			glVertex3f(0.5, 0.5, 0);
			glTexCoord2f(0, 1);
			glVertex3f(-0.5, 0.5, 0);

			//A quad on the yz-plane
			glTexCoord2f(0, 0);
			glVertex3f(0, -0.5, -0.5);
			glTexCoord2f(1, 0);
			glVertex3f(0, 0.5, -0.5);
			glTexCoord2f(1, 1);
			glVertex3f(0, 0.5, 0.5);
			glTexCoord2f(0, 1);
			glVertex3f(0, -0.5, 0.5);
		glEnd();
	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
}

//-- Updates the particle queue --------------------------------------------
void updateQueue() {
	const int LIFETIME = 3;
	list<particle>::iterator it;
	particle p;
	int tval;
	float delta;

	//Remove particles that have passed lifetime
	if (!parList.empty()) {
		p = parList.front();
		if (p.t > LIFETIME) parList.pop_front();
	}

	for (it = parList.begin(); it != parList.end(); it++) {
		tval = it->t;
		it->t = tval + 1;
		delta = (float)tval / (float)LIFETIME;

		for (int i = 0; i < 3; i++)	(it->pos[i]) += it->vel[i];

		it->size = delta * 1 + 0.5;	// (5 - 25)
		it->col = 1 - delta;		// (1 - 0)
	}

	if(tick % 2 == 0) newParticle();   //Create a new particle every sec.
}

void loadTexture() {
	glGenTextures(9, txId);

	glBindTexture(GL_TEXTURE_2D, txId[0]);
    loadTGA("Left.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[1]);
    loadTGA("Front.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[2]);
    loadTGA("Right.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[3]);
    loadTGA("Back.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[4]);
    loadTGA("Up.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[5]);
    loadTGA("Down.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, txId[6]);
    loadTGA("Path.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[7]);
    loadTGA("Ground.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	

	glBindTexture(GL_TEXTURE_2D, txId[8]);
	loadTGA("explosions3.tga");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void initialize(void) {
	float white[4]  = {1., 1., 1., 1.};
	glEnable(GL_LIGHTING);      //Enable OpenGL states
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 50);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glEnable(GL_LIGHT1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0);
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);
	loadTexture();

	glClearColor(0., 0., 0., 1.);
	// glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glBlendFunc(GL_ONE, GL_ONE);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60., 1., 1., 1000.); 
}


void skybox_left() {
	glBindTexture(GL_TEXTURE_2D, txId[0]);
	glColor3f(1, 1, 1);
	glNormal3f(1, 0, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-64, -64, 64);
		glTexCoord2f(1, 0);
		glVertex3f(-64, -64, -64);
		glTexCoord2f(1, 1);
		glVertex3f(-64, 64, -64);
		glTexCoord2f(0, 1);
		glVertex3f(-64, 64, 64);
	glEnd();
}

void skybox_front() {
	glBindTexture(GL_TEXTURE_2D, txId[1]);
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-64, -64, -64);
		glTexCoord2f(1, 0);
		glVertex3f(64, -64, -64);
		glTexCoord2f(1, 1);
		glVertex3f(64, 64, -64);
		glTexCoord2f(0, 1);
		glVertex3f(-64, 64, -64);
	glEnd();
}

void skybox_right() {
	glBindTexture(GL_TEXTURE_2D, txId[2]);
	glColor3f(1, 1, 1);
	glNormal3f(-1, 0, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(64, -64, -64);
		glTexCoord2f(1, 0);
		glVertex3f(64, -64, 64);
		glTexCoord2f(1, 1);
		glVertex3f(64, 64, 64);
		glTexCoord2f(0, 1);
		glVertex3f(64, 64, -64);
	glEnd();
}

void skybox_back() {
	glBindTexture(GL_TEXTURE_2D, txId[3]);
	glColor3f(1, 1, 1);
	glNormal3f(0, 0, -1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(64, -64, 64);
		glTexCoord2f(1, 0);
		glVertex3f(-64, -64, 64);
		glTexCoord2f(1, 1);
		glVertex3f(-64, 64, 64);
		glTexCoord2f(0, 1);
		glVertex3f(64, 64, 64);
	glEnd();
}

void skybox_up() {
	glBindTexture(GL_TEXTURE_2D, txId[4]);
	glColor3f(1, 1, 1);
	glNormal3f(0, -1, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-64, 64, -64);
		glTexCoord2f(1, 0);
		glVertex3f(64, 64, -64);
		glTexCoord2f(1, 1);
		glVertex3f(64, 64, 64);
		glTexCoord2f(0, 1);
		glVertex3f(-64, 64, 64);
	glEnd();
}

void skybox_down() {
	glBindTexture(GL_TEXTURE_2D, txId[5]);
	glColor3f(1, 1, 1);
	glNormal3f(0, 1, 0);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-64, -64, 64);
		glTexCoord2f(1, 0);
		glVertex3f(64, -64, 64);
		glTexCoord2f(1, 1);
		glVertex3f(64, -64, -64);
		glTexCoord2f(0, 1);
		glVertex3f(-64, -64, -64);
	glEnd();
}

void skybox_call() {
	glPushMatrix();
	glTranslatef(eye_x, 0, eye_z);
	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);

	skybox_left();
	skybox_front();
	skybox_right();
	skybox_back();
	skybox_up();
	skybox_down();

	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void drawFloor() {
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0., 0., 0., 1.};
	glBindTexture(GL_TEXTURE_2D, txId[7]);

	glColor4f(0.5, 0.5, 0.5, 1);     //Floor colour
	glNormal3f(0,1,0);
	
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	glBegin(GL_QUADS);
		for (float i = -16; i < 16; i+=1) {
			for (float j = -16; j < 16; j+=1) {
				if ((i < -2.3) || (i > 1.8)) {
					glTexCoord2f(0, 0);
					glVertex3f(i, 0, j);
					glTexCoord2f(1, 0);
					glVertex3f(i, 0, j+1);
					glTexCoord2f(1, 1);
					glVertex3f(i+1, 0, j+1);
					glTexCoord2f(0, 1);
					glVertex3f(i+1, 0, j);
				}
			}
		}
	glEnd();
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

void path() {
	float white[4] = {1., 1., 1., 1.};
	float black[4] = {0., 0., 0., 1.};
	glBindTexture(GL_TEXTURE_2D, txId[6]);
	glColor4f(0.5, 0.5, 0.5, 1);
	glNormal3f(0,1,0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);
	glBegin(GL_QUADS);
		for (float i = -2; i < 1.2; i+=1) {
			for (float j = -16; j < 16; j+=1) {
				glTexCoord2f(0, 0);
				glVertex3f(i, 0.0, j);
				glTexCoord2f(1, 0);
				glVertex3f(i, 0.0, j+1);
				glTexCoord2f(1, 1);
				glVertex3f(i+1, 0.0, j+1);
				glTexCoord2f(0, 1);
				glVertex3f(i+1, 0.0, j);				
			}
		}
	glEnd();
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
}

void alien() {
	glPushMatrix(); //main body
		glColor3f(0.96, 0.94, 0.6);
		glScalef(1.6,2,1.6);
		glutSolidOctahedron();
	glPopMatrix();

	glPushMatrix(); //belt
		glColor3f(0.1, 0.1, 0.1);
		glScalef(1.8,0.5,1.8);
		glutSolidOctahedron();
	glPopMatrix();
	
	glPushMatrix(); // head
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		glColor3f(0.9, 0.94, 0.6);
		glTranslatef(0,3,0);
		glutSolidOctahedron();
		glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);
	glPopMatrix();

	glPushMatrix(); // left eye
		glColor3f(0.1, 0.1, 0.1);
		glTranslatef(0.4, 3.4, 0.6);
		glScalef(0.15, 0.15, 0.15);
		glutSolidIcosahedron();
	glPopMatrix();

	glPushMatrix(); // right eye
		glColor3f(0.1, 0.1, 0.1);
		glTranslatef(-0.4, 3.4, 0.6);
		glScalef(0.15, 0.15, 0.15);
		glutSolidIcosahedron();
	glPopMatrix();
}

void alien_arms(int waving_flag) { // waving_flag: 0 for static, > 0 for moving
	glMaterialfv(GL_FRONT, GL_EMISSION, emission);
	glPushMatrix(); // left fingers
		glColor3f(0.9, 0.94, 0.6);
		if (waving_flag) glRotatef(-30+anim_hand, 1, 0, 0);
		glRotatef(-30, 0, 1, 0);
		glRotatef(15, 0, 0, 1);
		glTranslatef(2.4, 0, 0);
		glutSolidCube(0.8);
	glPopMatrix();

	glPushMatrix(); // right fingers
		glColor3f(0.9, 0.94, 0.6);
		if (waving_flag) glRotatef(-30-anim_hand, 1, 0, 0);
		glRotatef(30, 0, 1, 0);
		glRotatef(-15, 0, 0, 1);
		glTranslatef(-2.4 ,0, 0);
		glRotatef(180, 0, 1, 0);
		glutSolidCube(0.8);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_EMISSION, no_emission);

	glPushMatrix(); // right hand
		glColor3f(0.9, 0.94, 0.6);
		if (waving_flag) glRotatef(-30-anim_hand, 1, 0, 0);
		glRotatef(30, 0, 1, 0);
		glRotatef(-15, 0, 0, 1);
		glTranslatef(-2 ,0, 0);
		glRotatef(180, 0, 1, 0);
		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix(); // left hand
		glColor3f(0.9, 0.94, 0.6);
		if (waving_flag) glRotatef(-30+anim_hand, 1, 0, 0);
		glRotatef(-30, 0, 1, 0);
		glRotatef(15, 0, 0, 1);
		glTranslatef(2 ,0, 0);
		glutSolidCube(1);
	glPopMatrix();
}

void ferris_wheel() {

	GLUquadric *q;
	q = gluNewQuadric();

	glPushMatrix(); // right leg
		glColor3f(1.0, 0.4588, 0.0);
		glTranslatef(0.6,2.2,-10);
		glRotatef(35, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(q, 0.2, 0.2, 3, 10, 10);
		gluQuadricDrawStyle(q, GLU_FILL);
	glPopMatrix();

	glPushMatrix(); // left leg
		glColor3f(1.0, 0.4588, 0.0);
		glTranslatef(-0.6,2.2,-10);
		glRotatef(-35, 0, 0, 1);
		glRotatef(90, 1, 0, 0);
		gluCylinder(q, 0.2, 0.2, 3, 10, 10);
		gluQuadricDrawStyle(q, GLU_FILL);
	glPopMatrix();

	glPushMatrix(); // front cone
		glColor3f(.55,.57,.55);
		glTranslatef(0,2.2,-9.8);
		glutSolidCone(1, .5, 20, 4);
	glPopMatrix();

	glPushMatrix(); // centre
		glColor3f(.68,.85,.90);
		glTranslatef(0,2.2,-10);
		gluSphere(q, .7, 20, 20);
		gluQuadricDrawStyle(q, GLU_FILL);
	glPopMatrix();

	glPushMatrix(); // back cone
		glColor3f(.55,.57,.55);
		glTranslatef(0,2.2,-10.2);
		glRotatef(180, 1, 0, 0);
		glutSolidCone(1, .5, 20, 4);
	glPopMatrix();

	for (int i=0; i<6; i++) {
		glPushMatrix(); //spoke
			glColor3f(.55,.57,.55);
			glTranslatef(-0,2.2,-9.5);
			glRotatef(60*i+anim_wheel, 0, 0, 1);
			glRotatef(-90, 1, 0, 0);
			gluCylinder(q, 0.1, 0.1, 2, 10, 10);
			gluQuadricDrawStyle(q, GLU_FILL);
		glPopMatrix();

		glPushMatrix(); // seats
			glColor3f(.4,.57,.55);
			glTranslatef(0,4.2,-9.1);
			glTranslatef(0, -2, 0);
			glRotatef(-60*i+anim_wheel, 0, 0, 1);
			glTranslatef(0, 2, 0);
			glRotatef(60*i-anim_wheel, 0, 0, 1);
			glScalef(1, 0.2, 1);
			glutSolidCube(1);
		glPopMatrix();
	}

}

void pillar() {
	glPushMatrix(); // base
		glColor3f(0.8, 0.4, 0.8);
		glTranslatef(-3, 1, -2);
		glScalef(0.5, 2, 0.5);
		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix(); // top
		glColor3f(1, 0.2, 0.2);
		glTranslatef(4, .6, -2);
		glutSolidSphere(0.6, 36, 20);
	glPopMatrix();

	glDisable(GL_LIGHTING);
	glPushMatrix(); // base shadow
		glColor3f(0., 0., 0.);
		glTranslatef(0, 0.01, 0);
		glMultMatrixf(shadowMat);
		glTranslatef(-3, 1, -2);
		glScalef(0.5, 2, 0.5);
		glutSolidCube(1);
	glPopMatrix();

	glPushMatrix(); // top shadow
		glColor3f(0., 0., 0.);
		glTranslatef(0, 0.01, 0);
		glMultMatrixf(shadowMat);
		glTranslatef(4, .6, -2);
		glutSolidSphere(0.6, 36, 20);
	glPopMatrix();
	glEnable(GL_LIGHTING);

}

void special(int key, int x, int y) {
	if		(key == GLUT_KEY_LEFT)  turnDirection = -1;
	else if (key == GLUT_KEY_RIGHT) turnDirection = 1;
	else if (key == GLUT_KEY_DOWN) moveDirection = -1;
	else if (key == GLUT_KEY_UP) moveDirection = 1;

	glutPostRedisplay();
}

void specialUp(int key, int x, int y) {
	if		(key == GLUT_KEY_LEFT)  turnDirection = 0;
	else if (key == GLUT_KEY_RIGHT) turnDirection = 0;
	else if (key == GLUT_KEY_DOWN) moveDirection = 0;
	else if (key == GLUT_KEY_UP) moveDirection = 0;
}

void keyboard_handler(unsigned char key, int x, int y) {
	static int toggled = 0;
    if (key == 'q') {
        toggled = !toggled;

        if (toggled) {
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glEnable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}

void display(void) { 
	float spotdir[] = {0.0, -1, 1};
	float spotpos[] = {0, 3.4, 0.6, 1};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eye_x, 2, eye_z, look_x, 2, look_z, 0, 1, 0);

	
	glLightfv(GL_LIGHT0,GL_POSITION, light);

	skybox_call();
	drawFloor();
	path();
	glDisable(GL_TEXTURE_2D);
	

	glPushMatrix(); // alien on ferris wheel
		glTranslatef(0, -(alien_height/40), (board/20));
		glTranslatef(0,0.7,-9.1);
		if (!grounded) {
			glTranslatef(0, 2, 0);
			glRotatef(anim_wheel, 0, 0, 1);
			glTranslatef(0, -2, 0);
			glRotatef(-anim_wheel, 0, 0, 1);
		}
		glRotatef(alien_rotate, 0, 1, 0);
		if (grounded) {
			list<particle>::iterator it1;
			for (it1 = parList.begin(); it1 != parList.end(); it1++) {
				drawParticle(it1->col, it1->size, it1->pos[0], it1->pos[1], it1->pos[2]);
			}
		}
		glScalef(0.2, 0.2, 0.2);
		alien_arms(1);
		alien();
	glPopMatrix();

	glPushMatrix(); //alien on ground
		// glRotatef(alien_move/2, 0, 1, 0);
		glTranslatef(1, 0.5, -6+(alien_move/20));
		glRotatef(alien_turn*3.6, 0, 1, 0);

		list<particle>::iterator it;
		for (it = parList.begin(); it != parList.end(); it++) {
			drawParticle(it->col, it->size, it->pos[0], it->pos[1], it->pos[2]);
		}

		glScalef(0.2, 0.2, 0.2);
		alien_arms(0);
		alien();
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);
		glLightfv(GL_LIGHT1, GL_POSITION, spotpos);
	glPopMatrix();

	ferris_wheel();

	pillar();

	glEnable(GL_TEXTURE_2D);
	// glFlush(); 
	glutSwapBuffers();
}

void myTimer(int value) {
	value += 1;
	value2 += 1;

	tick++;
	if (tick == INT_MAX) tick = 0;

	if ((check == 1) && (anim_hand < 30)) {
		anim_hand = anim_hand + 2;
	} else {
		check = 0;
	}
	
	if ((check == 0) && (anim_hand > -30)) {
		anim_hand = anim_hand - 2;
	} else {
		check = 1;
	}
	
	if (value < 100) {
		//move alien on ground forward
		alien_move+=1;
	} else if (value < 150) {
		// turn
		alien_turn+=1;
	} else if (value < 250) {
		alien_move-=1;
	} else if (value < 300) {
		alien_turn-=1;
	} else {
		value = 0;
	}

	if (value2 < 360) {

	} else if (value2 < 540 && value2 >= 360) {
		grounded = 1;
		anim_wheel -= 1;
		if (board < 20) {
			board += 1;
			alien_height += 0.5;
		}
		if (alien_rotate < 180) {
			alien_rotate += 1;
		}
	} else if (value2 < 900) {

	} else if (value2 < 1080 && value2 >= 900) {
		grounded = 0;
		anim_wheel -= 1;
		if (board > 0) {
			board -= 1;
			alien_height -= 0.5;
		}
		if (alien_rotate > 0) {
			alien_rotate -= 1;
		}
		
	} else {
		value2 = 0;
	}

	angle += turnDirection * 0.05;
	eye_x += moveDirection * 0.3*sin(angle);
	eye_z -= moveDirection * 0.3*cos(angle);
	look_x = eye_x + 20*sin(angle);
	look_z = eye_z - 20*cos(angle);

	anim_wheel += 1;
	updateQueue();
	glutPostRedisplay();
	glutTimerFunc(16, myTimer, value);
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Alien");
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutTimerFunc(1, myTimer, 0);
	glutKeyboardFunc(keyboard_handler);
	initialize();

	glutDisplayFunc(display);
	glutMainLoop();
	return 0; 
}