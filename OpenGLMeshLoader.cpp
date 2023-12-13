#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include <glut.h>
#include <vector>

int WIDTH = 1280;
int HEIGHT = 720;

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
void updateCameraView() {
    // Define the camera position and orientation
    float cameraX = playerX;
    float cameraY = playerY + 1.0f; // Adjust the camera height
    float cameraZ = playerZ + 5.0f; // Adjust the camera distance

    // Set the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, playerX, playerY, playerZ, 0.0f, 1.0f, 0.0f);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// Update the camera view
	updateCameraView();

	// Draw your scene objects here

	glutSwapBuffers();
}
	

Vector Eye(20, 5, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);
Vector playerPosition = { 2.0f, 0.0f, 1.0f };

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_player;
// Textures
GLTexture tex_ground;
Vector getPlayerPosition() {
	return playerPosition;
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
		gluLookAt(posX, posY, posZ, lookX, lookY, lookZ, upX, upY, upZ);
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

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
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

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	GLfloat lightIntensity[] = { 0.7, 0.7, 0.7, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);
	camera.setPosition(player.getPosition()[0], player.getPosition()[1], player.getPosition()[2]);
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
	glTranslatef(2, 0, 4);
	glRotatef(0.f, 1, 0, 0);
	glScalef(0.1f, 0.1f, 0.1f);
	model_player.Draw();
	glPopMatrix();


	// Draw house Model
	glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();


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



	glutSwapBuffers();
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
	case 'a':
		model_player.pos.x -= 1.0f; // Move left along the X-axis
		model_player.rot.y = -90.0f; // Rotate the player to face left
		if (model_player.pos.x < -450.0f) model_player.pos.x = -450.0f; // Boundary check
		break;
	case 'd':
		model_player.pos.x += 1.0f; // Move right along the X-axis
		model_player.rot.y = 90.0f; // Rotate the player to face right
		if (model_player.pos.x > 450.0f) model_player.pos.x = 450.0f; // Boundary check
		break;
	case 's':
		model_player.pos.z -= 1.0f; // Move backward along the Z-axis
		model_player.rot.y = 180.0f; // Rotate the player to face backward
		if (model_player.pos.z < -450.0f) model_player.pos.z = -450.0f; // Boundary check
		break;
	case 'w':
		model_player.pos.z += 1.0f; // Move forward along the Z-axis
		model_player.rot.y = 0.0f; // Rotate the player to face forward
		if (model_player.pos.z > 450.0f) model_player.pos.z = 450.0f; // Boundary check
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}

	glutPostRedisplay();
}
//=======================================================================
// Motion Function
//=======================================================================
void myMotion(int x, int y)
{
	y = HEIGHT - y;

	if (cameraZoom - y > 0)
	{
		Eye.x += -0.1;
		Eye.z += -0.1;
	}
	else
	{
		Eye.x += 0.1;
		Eye.z += 0.1;
	}

	cameraZoom = y;

	glLoadIdentity();	//Clear Model_View Matrix

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters

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
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
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

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(100, 150);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutMotionFunc(myMotion);
	glutDisplayFunc(renderScene);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}