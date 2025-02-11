#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>




// Global variables for transformations
float translateX = 0.0f, translateY = 0.0f, translateZ = 0.0f;
float rotateAngle = 0.0f, rotateX = 0.0f, rotateY = 0.0f, rotateZ = 0.0f;
float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
float colorR = 1.0f, colorG = 1.0f, colorB = 1.0f;


void changeSize(int w, int h) {
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if(h == 0)
		h = 1;

	// compute window's aspect ratio 
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(45.0f ,ratio, 1.0f ,1000.0f);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}


void renderScene(void) {

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(6.0,6.0,6.0, 
		      0.0,0.0,0.0,
			  0.0f,1.0f,0.0f);

// put axis drawing in here
	glBegin(GL_LINES);
		// X axis in red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		// Y Axis in Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f, 100.0f, 0.0f);
		// Z Axis in Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();



// put the geometric transformations here


// put pyramid drawing instructions here
	
	glBegin(GL_TRIANGLES); // (x,y,z)

		// Bottom square
		glColor3f(2.0f, 0.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);

		
		// Front face
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);

		// Right face
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);

		// Back face
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);

		// Left face
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 2.0f, 0.0f);
		glVertex3f(-1.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 0.0f, 1.0f);




	glEnd();

	// End of frame
	glutSwapBuffers();
}



// write function to process keyboard events

// write function to process keyboard events
void processNormalKeys(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': translateY += 0.1f; break;
	case 's': translateY -= 0.1f; break;
	case 'a': translateX -= 0.1f; break;
	case 'd': translateX += 0.1f; break;
	case 'q': translateZ += 0.1f; break;
	case 'e': translateZ -= 0.1f; break;
	case 'r': rotateAngle += 5.0f; break;
	case 'f': rotateAngle -= 5.0f; break;
	case 't': scaleX += 0.1f; scaleY += 0.1f; scaleZ += 0.1f; break;
	case 'g': scaleX -= 0.1f; scaleY -= 0.1f; scaleZ -= 0.1f; break;
	case '1': colorR = 1.0f; colorG = 0.0f; colorB = 0.0f; break;
	case '2': colorR = 0.0f; colorG = 1.0f; colorB = 0.0f; break;
	case '3': colorR = 0.0f; colorG = 0.0f; colorB = 1.0f; break;
	case '4': colorR = 1.0f; colorG = 1.0f; colorB = 1.0f; break;
	case 27: exit(0); break; // ESC key
	}
	glutPostRedisplay();
}








int main(int argc, char **argv) {

// init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(800,800);
	glutCreateWindow("CG@DI-UM");
		
// Required callback registry 
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);

	
// put here the registration of the keyboard callbacks



//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
// enter GLUT's main cycle
	glutMainLoop();
	
	return 1;
}
