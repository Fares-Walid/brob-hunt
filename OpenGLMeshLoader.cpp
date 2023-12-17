#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <vector>
#include <iostream>
#include <Windows.h>

int WIDTH = 1280;
int HEIGHT = 720;
float cameraOffsetY = 0;
GLuint tex;
char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;
float playerX = 2.0f;
float playerY = 0.0f;
float playerZ = 1.0f;
float cameraOffsetX = 0.1f;
float cameraOffsetZ = 0.0f;
float cameraMoveX = 0.0f;
float cameraMoveY = 0.0f;
float cameraMoveZ = 0.0f;
bool thirdPerson = true;
float animationRot = 0;
bool colaTarget = false;
bool skullTarget = false;
bool colaCollected = false;
bool skullCollected = false;
bool clocktarget = false;
bool clockCollected = false;
bool rocktarget = false;
bool rockCollected = false;
int collectedItems = 0;
bool level1 = true;
bool level2 = true;
bool cola2Collected = false;
bool skull2Collected = false;


class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};
void updateCameraView(float x, float y, float z) {
	// Define the camera position and orientation
	float cameraX = x / 10 + 1.6f;
	float cameraY = y / 10 + 3.6f; // Adjust the camera height
	float cameraZ = z / 10 + 1; // Adjust the camera distance

	// Set the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(cameraX + cameraMoveX, cameraY, cameraZ + cameraMoveZ, cameraX + cameraMoveX - cameraOffsetX, cameraY + cameraOffsetY, cameraZ + cameraMoveZ - cameraOffsetZ, 0.0f, 1.0f, 0.0f);
}


//void renderScene() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//
//	// Update the camera view
//	updateCameraView();
//
//	// Draw your scene objects here
//
//	glutSwapBuffers();
//}


Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);
Vector playerPosition = { 2.0f, 0.0f, 1.0f };

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_player;
Model_3DS model_skull;
Model_3DS model_bara;
Model_3DS model_bara2;
Model_3DS model_cont;
Model_3DS model_rock;
Model_3DS model_clock;
// Textures
GLTexture tex_ground;
GLTexture tex_skull;
GLTexture tex_bara;
GLTexture tex_cont;

Vector getPlayerPosition() {
	return playerPosition;
}
class V3 {
public:
	float x, y, z;

	V3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	V3 operator+(V3& v) {
		return V3(x + v.x, y + v.y, z + v.z);
	}

	V3 operator-(V3& v) {
		return V3(x - v.x, y - v.y, z - v.z);
	}

	V3 operator*(float n) {
		return V3(x * n, y * n, z * n);
	}

	V3 operator/(float n) {
		return V3(x / n, y / n, z / n);
	}

	V3 unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	V3 cross(V3 v) {
		return V3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

V3 calculateHUDPosition() {
	// Calculate the position of the HUD in front of the camera
	V3 HUDPosition = V3(model_player.pos.x, model_player.pos.y, model_player.pos.z);  // Adjust the distance as needed
	return HUDPosition;
}
float calculateDistance(float offx, float offz) {
	// Get the positions of the two models
	float x1 = model_player.pos.x + playerX;
	float z1 = model_player.pos.z + playerZ;



	// Calculate the Euclidean distance between the positions
	float distance = sqrt(pow(x1 - offx, 2) + pow(z1 - offz, 2));

	return distance;
}
//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

class Camera {
private:
	float posX, posY, posZ;        // Camera position
	float lookX, lookY, lookZ;     // Look-at position
	float upX, upY, upZ;           // Up vector

public:
	// Constructor
	Camera() {}

	// Set the camera position
	void setPosition(float x, float y, float z) {
		posX = x;
		posY = y;
		posZ = z;
	}

	// Set the look-at position
	void setLookAt(float x, float y, float z) {
		lookX = x;
		lookY = y;
		lookZ = z;
	}

	// Set the up vector
	void setUp(float x, float y, float z) {
		upX = x;
		upY = y;
		upZ = z;
	}

	// Move the camera with the player
	void moveWithPlayer(float playerX, float playerY, float playerZ) {
		posX = playerX;
		posY = playerY + 1.0f; // Adjust the camera height
		posZ = playerZ;
		lookX = playerX;
		lookY = playerY;
		lookZ = playerZ - 1.0f; // Look slightly ahead of the player
	}

	// Apply the camera transformations
	void apply() {
		//gluLookAt(posX, posY, posZ, lookX, lookY, lookZ, upX, upY, upZ);
	}
};
class Player {
private:
	std::vector<float> position; // Player position

public:
	// Constructor
	Player(float x = 1.0f, float y = 0.0f, float z = 2.0f) : position({ x, y, z }) {}

	// Move the player
	void move(float deltaX, float deltaY, float deltaZ) {
		position[0] += deltaX;
		position[1] += deltaY;
		position[2] += deltaZ;
	}

	// Get the player position
	const std::vector<float>& getPosition() const { return position; }
};

// Global camera and player objects
Camera camera;
Player player;
//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}


// mouse control
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_MOUSEMOVE) {
			// Extract the mouse coordinates from the lParam
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);

			// Output the mouse coordinates
			std::cout << "Mouse moved to: (" << x << ", " << y << ")" << std::endl;
		}
	}

	// Call the next hook procedure in the hook chain
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-50, 0, -50);
	glTexCoord2f(5, 0);
	glVertex3f(50, 0, -50);
	glTexCoord2f(5, 5);
	glVertex3f(50, 0, 50);
	glTexCoord2f(0, 5);
	glVertex3f(-50, 0, 50);
	glEnd();
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}
void update(int value) {
	animationRot += 0.01;
	glutPostRedisplay();
	glutTimerFunc(16, update, 0);
}
//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	if (level1) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
		//camera.setPosition(player.getPosition()[0], player.getPosition()[1], player.getPosition()[2]);
		// Draw Ground
		RenderGround();




		// Draw Tree Model
		glPushMatrix();
		glTranslatef(10, 0, 0);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(4.0f, 0.0f, 10.0f);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();
		// Draw Tree Model
		glPushMatrix();
		glTranslatef(10.0f, 0.0f, 6.0f);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();
		// Draw Tree Model
		glPushMatrix();
		glTranslatef(15.0f, 0.0f, 8.0f);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();
		// Draw Tree Model
		glPushMatrix();
		glTranslatef(20.0f, 0.0f, 10.0f);
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();
		// Draw Player Model
		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(0.f, 1, 0, 0);
		glScalef(0.1f, 0.1f, 0.1f);
		model_player.Draw();
		glPopMatrix();


		if (!skullCollected) {
			// Draw skull Model
			glPushMatrix();
			//glRotatef(90.f, 1, 0, 0);
			glScalef(0.5f, 0.5f, 0.5f);
			glTranslatef(20, 1, 23);
			glRotated(360 * sin(animationRot), 0, 1, 0);

			model_skull.Draw();
			glPopMatrix();
		}

		// Draw container Model
		glPushMatrix();
		glTranslatef(20, 1, 30);
		glScalef(0.01f, 0.01f, 0.01f);
		
		//glRotatef(90.f, 1, 0, 0);
		
		glRotated(360 * sin(animationRot), 0, 1, 0);

		model_cont.Draw();
		glPopMatrix();






		// Draw house Model
		glPushMatrix();
		glRotatef(90.f, 1, 0, 0);
		model_house.Draw();
		glPopMatrix();

		// Draw house 2 Model
		glPushMatrix();
		glColor3f(0.4, 0.96, 0.66);
		glTranslatef(45, 0, 45);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);


		model_house.Draw();
		glPopMatrix();

		if (!colaCollected) {
			// Draw cola Model
			glPushMatrix();
			glTranslatef(30, 0.3, 30);
			glRotatef(270.f, 1, 0, 0);

			glScalef(0.01f, 0.01f, 0.01f);
			glRotated(360 * sin(animationRot), 0, 1, 0);


			model_bara.Draw();
			glPopMatrix();

			// Draw cola2 Model
			glPushMatrix();
			//glRotatef(90.f, 1, 0, 0);
			glTranslatef(40, 0.3, 50);
			glScalef(0.01f, 0.01f, 0.01f);

			glRotated(360 * sin(animationRot), 0, 1, 0);

			model_bara2.Draw();
			glPopMatrix();
		}

		//sky box
		glPushMatrix();

		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);


		glPopMatrix();



		glPushMatrix();

		// Translate the scene for HUD rendering
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
		glDisable(GL_DEPTH_TEST);  // disable depth-testing

		V3 HUDPosition = calculateHUDPosition();
		glRasterPos2f(HUDPosition.x, HUDPosition.y);
		glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white
		char* textForTarget;
		if (colaCollected && skullCollected) {
			textForTarget = "Go to the green house";
		}
		else if (colaTarget) {
			textForTarget = "Get the cola";
		}
		else {
			textForTarget = "Get the skull";
		}


		char targetText[100];
		sprintf(targetText, "%s", textForTarget);
		char collectedText[100];  // Adjust the size based on your needs
		if (collectedItems > 2) {
			collectedItems = 2;
		}
		sprintf(collectedText, "                  Collected Items: %i", collectedItems);

		// Concatenate the collected items text to the target text
		strcat(targetText, collectedText);
		int len = strlen(targetText);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, targetText[i]);
		}

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		// Restore the matrix state
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glutPostRedisplay();
		glutSwapBuffers();
	}
	else if (level2) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		GLfloat lightIntensity[] = { 0.2, 0.2, 0.2, 1.0f };
		GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
		//camera.setPosition(player.getPosition()[0], player.getPosition()[1], player.getPosition()[2]);
		// Draw Ground
		RenderGround();





		// Draw Tree Model
		glPushMatrix();
		glTranslatef(0, 0, 10);  // Switched x and z coordinates
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(10.0f, 0.0f, 4.0f);  // Switched x and z coordinates
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(6.0f, 0.0f, 10.0f);  // Switched x and z coordinates
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(8.0f, 0.0f, 15.0f);  // Switched x and z coordinates
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Tree Model
		glPushMatrix();
		glTranslatef(10.0f, 0.0f, 20.0f);  // Switched x and z coordinates
		glScalef(0.7, 0.7, 0.7);
		model_tree.Draw();
		glPopMatrix();

		// Draw Player Model
		glPushMatrix();
		glTranslatef(playerX, playerY, playerZ);
		glRotatef(0.f, 1, 0, 0);
		glScalef(0.1f, 0.1f, 0.1f);
		GLfloat lightIntensity2[] = { 0.6, 0.6, 0.6, 1.0f };
		GLfloat lightPosition2[] = { 0.0f, 1.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition2);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity2);
		model_player.Draw();
		glPopMatrix();
		// Draw rockModel


		glPushMatrix();
		glScalef(0.1f, 0.1f, 0.1f); // Switched x and z coordinates
		glTranslatef(03, 1, 40);
		//glRotated(360 * sin(animationRot), 0, 1, 0);
		model_rock.Draw();
		glPopMatrix();
		if (!rockCollected) {
			// Draw rockModel


			glPushMatrix();
			glScalef(0.1f, 0.7f, 0.1f); // Switched x and z coordinates
			glTranslatef(23, 1, 20);
			glRotated(360 * sin(animationRot), 0, 1, 0);
			model_bara.Draw();
			glPopMatrix();
		}

		// Draw container Model
		glPushMatrix();

		glTranslatef(30, 1, 20);  // Switched x and z coordinates

		glScalef(0.01f, 0.01f, 0.01f);
		glRotated(360 * sin(animationRot), 0, 1, 0);
		model_cont.Draw();
		glPopMatrix();

		// Draw house 2 Model
		glPushMatrix();
		glColor3f(0.4, 0.96, 0.66);
		glTranslatef(45, 0, 45);
		glRotatef(90.f, 1, 0, 0);
		glRotatef(180.f, 0, 0, 1);
		model_house.Draw();
		glPopMatrix();

		if (!clockCollected) {
			// Draw clock Model
			glPushMatrix();
			glTranslatef(30, 0.3, 30);
			//glRotatef(270.f, 1, 0, 0);
			glScalef(0.1f, 0.1f, 0.1f);
			glRotated(360 * sin(animationRot), 0, 1, 0);
			model_clock.Draw();
			glPopMatrix();


		}

		//sky box
		glPushMatrix();
		GLUquadricObj* qobj;
		qobj = gluNewQuadric();
		glTranslated(0, 0, 50);  // Switched x and z coordinates
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
		glPopMatrix();


		glPushMatrix();

		// Translate the scene for HUD rendering
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
		glDisable(GL_DEPTH_TEST);  // disable depth-testing

		V3 HUDPosition = calculateHUDPosition();
		glRasterPos2f(HUDPosition.x, HUDPosition.y);
		glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white
		char* textForTarget;


		if (colaCollected && skullCollected && level1 || clockCollected && rockCollected) {
			textForTarget = "Go to the green house";
		}
		else if (clocktarget) {
			textForTarget = "Get the clock";
		}
		else {
			textForTarget = "Get the rock";
		}
		char targetText[100];
		sprintf(targetText, "%s", textForTarget);
		char collectedText[100];  // Adjust the size based on your needs
		if (collectedItems > 4) {
			collectedItems = 4;
			if (level1)
				collectedItems = 2;
		}
		sprintf(collectedText, "                  Collected Items: %i", collectedItems);

		// Concatenate the collected items text to the target text
		strcat(targetText, collectedText);
		int len = strlen(targetText);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, targetText[i]);
		}

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		// Restore the matrix state
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glutPostRedisplay();
		glutSwapBuffers();
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();

		// Translate the scene for HUD rendering
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WIDTH, 0, HEIGHT);

		glDepthMask(GL_FALSE);  // disable writes to Z-Buffer
		glDisable(GL_DEPTH_TEST);  // disable depth-testing

		V3 HUDPosition = calculateHUDPosition();
		glRasterPos2f(HUDPosition.x, HUDPosition.y);
		glColor3f(1.0f, 1.0f, 1.0f);  // Set color to white
		char* textForTarget;

		textForTarget = "Your winner";


		char targetText[100];
		sprintf(targetText, "%s", textForTarget);

		// Concatenate the collected items text to the target text

		int len = strlen(targetText);
		for (int i = 0; i < len; i++) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, targetText[i]);
		}
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		// Restore the matrix state
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glutPostRedisplay();
		glutSwapBuffers();
	}
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char button, int x, int y)
{
	switch (button)
	{
	case 'e':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'd':
		model_player.pos.x -= 5.0f; // Move left along the X-axis
		model_player.rot.y = -90.0f; // Rotate the player to face left


		cameraOffsetX = 0.1f;
		cameraOffsetZ = 0.0f;
		if (thirdPerson) {
			cameraMoveX = 3;
			cameraMoveY = 0.5f;
			cameraMoveZ = 0;
		}
		if (model_player.pos.x < -450.0f) model_player.pos.x = -450.0f; // Boundary check
		break;
	case 'a':
		model_player.pos.x += 5.0f; // Move right along the X-axis
		model_player.rot.y = 90.0f; // Rotate the player to face right
		cameraOffsetX = -0.1f;
		cameraOffsetZ = 0.0f;
		if (thirdPerson) {
			cameraMoveX = -3;
			cameraMoveY = 0.5f;
			cameraMoveZ = 0;
		}
		if (model_player.pos.x > 450.0f) model_player.pos.x = 450.0f; // Boundary check
		break;
	case 's':
		model_player.pos.z -= 5.0f; // Move backward along the Z-axis
		model_player.rot.y = 180.0f; // Rotate the player to face backward

		cameraOffsetX = 0.0f;
		cameraOffsetZ = 0.1f;
		if (thirdPerson) {
			cameraMoveX = 0;
			cameraMoveY = 0.5f;
			cameraMoveZ = 3;
		}
		if (model_player.pos.z < -450.0f) model_player.pos.z = -450.0f; // Boundary check
		break;
	case 'w':
		model_player.pos.z += 5.0f; // Move forward along the Z-axis
		model_player.rot.y = 0.0f; // Rotate the player to face forward
		cameraOffsetX = 0.0f;
		cameraOffsetZ = -0.1f;
		if (thirdPerson) {
			cameraMoveX = 0;
			cameraMoveY = 0.5f;
			cameraMoveZ = -3;
		}
		if (model_player.pos.z > 450.0f) model_player.pos.z = 450.0f; // Boundary check
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
	if (level1 && colaTarget && calculateDistance(275, 290) < 20) {
		colaTarget = false;
		colaCollected = true;
		skullTarget = true;
		collectedItems += 1;

		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-unlock-game-notification-253.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
	}
	if (level1 && skullTarget && calculateDistance(75, 95) < 20) {
		colaTarget = true;
		skullCollected = true;
		skullTarget = false;
		collectedItems += 1;

		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-unlock-game-notification-253.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
	}
	if (level1 && colaCollected && skullCollected && calculateDistance(410, 425) < 30) {
		level1 = false;
		srand(static_cast<unsigned int>(time(0)));
		int randN = rand() % 2;
		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-achievement-completed-2068.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
		if (randN == 0) {
			clocktarget = true;
		}
		else {
			rocktarget = true;
		}
	}
	if (!level1 && clocktarget && calculateDistance(280, 300) < 20) {
		clocktarget = false;
		clockCollected = true;
		rockCollected = true;
		collectedItems += 1;

		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-unlock-game-notification-253.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
	}
	if (!level1 && rocktarget && calculateDistance(105, 95) < 20) {
		clocktarget = true;
		rockCollected = true;
		rocktarget = false;
		collectedItems += 1;

		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-unlock-game-notification-253.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
	}
	if (clockCollected && rockCollected && calculateDistance(410, 425) < 30) {
		level2 = false;
		std::cout << "playing music\n";
		PlaySound(TEXT("mixkit-achievement-completed-2068.wav"), NULL, SND_FILENAME | SND_ASYNC);
		std::cout << "music over \n";
	}
	std::cout << "x: " << model_player.pos.x << " z: " << model_player.pos.z << std::endl;
	updateCameraView(model_player.pos.x, model_player.pos.y, model_player.pos.z);


}
//=======================================================================
// Motion Function
//=======================================================================
void mouseButtonCallback(int button, int state, int x, int y) {
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		// Right mouse button down event
		thirdPerson = !thirdPerson;
		if (!thirdPerson) {
			cameraMoveX = 0;
			cameraMoveY = 0;
			cameraMoveZ = 0;
		}
		updateCameraView(model_player.pos.x, model_player.pos.y, model_player.pos.z);
	}

}

void myMotion(int x, int y) {


	std::cout << "x: " << x << " y: " << y << std::endl;

	if (y > 300) {
		cameraOffsetY -= 0.01f;
	}
	else {
		cameraOffsetY += 0.01f;
	}
	if (cameraOffsetY > 0.2f) {
		cameraOffsetY = 0.2f;
	}
	if (cameraOffsetY < -0.2f) {
		cameraOffsetY = -0.2f;
	}

	updateCameraView(model_player.pos.x, model_player.pos.y, model_player.pos.z);

	/*y = HEIGHT - y;*/

	//if (cameraZoom - y > 0)
	//{
	//	Eye.x += -0.1;
	//	Eye.z += -0.1;
	//}
	//else
	//{
	//	Eye.x += 0.1;
	//	Eye.z += 0.1;
	//}



	//glLoadIdentity();	//Clear Model_View Matrix

	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glutPostRedisplay();	//Re-draw scene 
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (state == GLUT_DOWN)
	{
		cameraZoom = y;
	}
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_player.Load("Models/player/human.3DS");
	model_skull.Load("Models/skull/skull.3dS");
	model_bara.Load("Models/hollow/Bottle Coca-Cola N080710.3ds");
	model_bara2.Load("Models/hollow/Bottle Coca-Cola N080710.3ds");
	model_cont.Load("Models/bara/Container.3DS");
	model_rock.Load("Models/rock/Rock04_Set.3ds");
	model_clock.Load("Models/clock/OldWallClock_3dsMax.3ds");


	// Loading texture files

	tex_ground.Load("Textures/ground.bmp");

	//tex_bara.Load("Textures/goku.bmp");
	//tex_skull.Load("Textures/front.bmp");
	//tex_skull.Load("Textures/down.bmp");
	//tex_skull.Load("Textures/side.bmp");
	//tex_skull.Load("Textures/top.bmp");
	//tex_skull.Load("Textures/up.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);

	if (!level1) {
		glColor3f(0.84, 0.49, 0.63);
		tex_ground.Load("Textures/ground.bmp");
		loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
	}
}

//=======================================================================
// Main Function
//=======================================================================
int main(int argc, char** argv)
{
	srand(static_cast<unsigned int>(time(0)));
	int randN = rand() % 2;
	if (randN == 0) {
		colaTarget = true;
	}
	else {
		skullTarget = true;
	}
	glutInit(&argc, argv);


	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);
	///glutDisplayFunc(renderScene);
	glutMouseFunc(myMouse);
	glutMouseFunc(mouseButtonCallback);

	glutTimerFunc(0, update, 0);


	glutReshapeFunc(myReshape);

	myInit();

	// Install the mouse hook
	HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);




	// Uninstall the mouse hook
	UnhookWindowsHookEx(mouseHook);

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);


	glutMainLoop();

	return 0;
}