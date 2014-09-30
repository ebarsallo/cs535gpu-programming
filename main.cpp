#ifdef _WIN32
#include <windows.h>
#endif

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glui.h>
#include "GPGPU.h"

static int winWidth = 512;
static int winHeight = 512;
static int winId = -1;
static GLUI *glui;

GPGPU  *gpgpu;

/*****************************************************************************
 * README  
 * ---------------------------------------------------------------------------
 * ebarsall
 * Assignment #2: GPU Programming
 *
 * 
 *****************************************************************************/

std::vector<GLUI_Spinner*> wAmpSpinnerCtrl;
std::vector<GLUI_Spinner*> wDxSpinnerCtrl;
std::vector<GLUI_Spinner*> wDySpinnerCtrl;
std::vector<GLUI_Spinner*> wSpeedSpinnerCtrl;
std::vector<GLUI_Spinner*> wWlSpinnerCtrl;

/*****************************************************************************
*****************************************************************************/
static void
leftButtonDownCB(void)
{
   gpgpu->restart();
}

/*****************************************************************************
*****************************************************************************/
static void
leftButtonUpCB(void)
{
}

/*****************************************************************************
*****************************************************************************/
static void
middleButtonDownCB(void)
{
}


/*****************************************************************************
*****************************************************************************/
static void
middleButtonUpCB(void)
{
}

/*****************************************************************************
*****************************************************************************/
static void
rightButtonDownCB(void)
{
}


/*****************************************************************************
*****************************************************************************/
static void
rightButtonUpCB(void)
{
}

/*****************************************************************************
*****************************************************************************/
static void
mouseCB(int button, int state, int x, int y)
{
   if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
      leftButtonDownCB();
   else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
      leftButtonUpCB();
   else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
      middleButtonDownCB();
   else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
      middleButtonUpCB();
   else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
      rightButtonDownCB();
   else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
      rightButtonUpCB();
}


/*****************************************************************************
*****************************************************************************/
static void
motionCB(int x, int y)
{
}


/*****************************************************************************
*****************************************************************************/
void
reshapeCB(int width, int height)
{
    if (height == 0) height = 1;
    
    glViewport(0, 0, width, height);
    
	// Use orthographic projection
    glMatrixMode(GL_PROJECTION);    
    glLoadIdentity();               
    gluOrtho2D(-1, 1, -1, 1);       
    glMatrixMode(GL_MODELVIEW);     
    glLoadIdentity();   
}

/*****************************************************************************
*****************************************************************************/
void
keyboardCB(unsigned char key, int x, int y)
{
}

/*****************************************************************************
*****************************************************************************/
void
idleFunc()
{		
	glutPostRedisplay();
}

/*****************************************************************************
*****************************************************************************/
void
refreshCB()
{
	for (int i = 0; i < 10; ++i) {
		// Update the cells' states
		gpgpu->update();  

		// Display the results
		gpgpu->display();
	}

    glutSwapBuffers();
}

/*****************************************************************************
*****************************************************************************/
void initialize()
{
    // Initialize glew library
    glewInit();

    // Create the gpgpu object
    gpgpu = new GPGPU(512, 512);
}

/*****************************************************************************
*****************************************************************************/
/**
 * <summary>Handler of all the GUI events.</summary>
 */
void 
GUICallbackHandler(int objID) 
{
	switch (objID) {
	}
}

void MakeGUI()
{
	glui = GLUI_Master.create_glui("GUI", 0, 0, 0);
	glui->add_statictext("GPGPU example");

	glui->set_main_gfx_window(winId);

	// gpu
	int count = 4;
	wAmpSpinnerCtrl.resize(count);
	wDxSpinnerCtrl.resize(count);
	wDySpinnerCtrl.resize(count);
	wSpeedSpinnerCtrl.resize(count);
	wWlSpinnerCtrl.resize(count);

	for (int i=0; i<count; i++) {
		glui->add_statictext ("Wave #" + i);

		GLUI_Spinner *t1Spinner = glui->add_spinner("Amplitude",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t1Spinner->set_int_limits( 0, 10, GLUI_LIMIT_CLAMP );	
			wAmpSpinnerCtrl[i] = t1Spinner;

		GLUI_Spinner *t2Spinner = glui->add_spinner("Direction X",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t2Spinner->set_int_limits( 0, 10, GLUI_LIMIT_CLAMP );	
			wDxSpinnerCtrl[i] = t2Spinner;

		GLUI_Spinner *t3Spinner = glui->add_spinner("Direction Y",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t3Spinner->set_int_limits( 0, 10, GLUI_LIMIT_CLAMP );	
			wDySpinnerCtrl[i] = t3Spinner;

		GLUI_Spinner *t4Spinner = glui->add_spinner("Speed",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t4Spinner->set_int_limits( 0, 10, GLUI_LIMIT_CLAMP );	
			wSpeedSpinnerCtrl[i] = t4Spinner;

		GLUI_Spinner *t5Spinner = glui->add_spinner("Wave Length",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t5Spinner->set_int_limits( 0, 10, GLUI_LIMIT_CLAMP );	
			wWlSpinnerCtrl[i] = t5Spinner;
	}


	/* We register the idle callback with GLUI, *not* with GLUT */
	GLUI_Master.set_glutIdleFunc(idleFunc);
}

/*****************************************************************************
*****************************************************************************/
int
main(int argc, char *argv[])
{
	// init OpenGL/GLUT
	glutInit(&argc, argv);
	
	// create main window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	winId = glutCreateWindow("Water Simulation");

	initialize();
	
	// setup callbacks
	glutDisplayFunc(refreshCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
	glutMotionFunc(motionCB);

	// force initial matrix setup
	reshapeCB(winWidth, winHeight);

	// set modelview matrix stack to identity
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// make GLUI GUI
	MakeGUI();
	glutMainLoop();

	return (TRUE);
}
