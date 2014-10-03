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
 *****************************************************************************/

std::vector<GLUI_Spinner*> gwAmpSpinnerCtrl;
std::vector<GLUI_Spinner*> gwDxSpinnerCtrl;
std::vector<GLUI_Spinner*> gwDySpinnerCtrl;
std::vector<GLUI_Spinner*> gwSpeedSpinnerCtrl;
std::vector<GLUI_Spinner*> gwWlSpinnerCtrl;

GLUI_Spinner* gwNWaves;
GLUI_Listbox* gwlbTWaves;

static int gcWaves = 8;
static int gtWaves = 0;


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

	// Update type of waves
	case 10:
		gtWaves = gwlbTWaves->get_int_val();
		gpgpu->setSinTWaves(gtWaves);
		refreshCB();
		break;

	// Update number of waves
	case 20:
		gcWaves = gwNWaves->get_int_val();
		gpgpu->setSinNWaves(gcWaves);
		refreshCB();
		break;

	// Update sine params
	case 30:
		float t1[10], t2[10], t3[10], t4[10], t5[10];
		for (int i=0; i<gcWaves; i++) {
			t1[i] = gwAmpSpinnerCtrl[i]->get_float_val();
			t2[i] = gwDxSpinnerCtrl[i]->get_float_val();
			t3[i] = gwDySpinnerCtrl[i]->get_float_val();
			t4[i] = gwSpeedSpinnerCtrl[i]->get_float_val();
			t5[i] = gwWlSpinnerCtrl[i]->get_float_val();
		}
		
		gpgpu->setSinParamAmplitude(t1);
		gpgpu->setSinParamDx(t2);
		gpgpu->setSinParamDy(t3);
		gpgpu->setSinParamSpeed(t4);
		gpgpu->setSinParamWaveLength(t5);

		refreshCB();
		break;

	default:
		break;
	}
}

/**
 * <summary>Sync (initial) values of GUI controls</summary>
 */
void 
syncVal ()
{
	gwNWaves->set_int_val(sinNWaves);

	for (int i=0; i<gcWaves; i++) {
		gwAmpSpinnerCtrl[i]->set_float_val(sinParamAmplitude[i]);
		gwDxSpinnerCtrl[i]->set_float_val(sinParamDx[i]);
		gwDySpinnerCtrl[i]->set_float_val(sinParamDy[i]);
		gwSpeedSpinnerCtrl[i]->set_float_val(sinParamSpeed[i]);
		gwWlSpinnerCtrl[i]->set_float_val(sinParamWaveLength[i]);
	}
}

void MakeGUI()
{
	glui = GLUI_Master.create_glui("GUI", 0, winWidth+30, 0);
	glui->add_statictext("Assignment #3");
	glui->add_statictext("GPU Programming");
	glui->add_statictext("");
	glui->add_statictext("Water simulation using");
	glui->add_statictext("Sine wave summation.");

	glui->add_separator();

	glui->set_main_gfx_window(winId);

	// gpu
	int count = gcWaves;

	gwAmpSpinnerCtrl.resize(count);
	gwDxSpinnerCtrl.resize(count);
	gwDySpinnerCtrl.resize(count);
	gwSpeedSpinnerCtrl.resize(count);
	gwWlSpinnerCtrl.resize(count);

	gwlbTWaves = glui->add_listbox("Type of Wave:",&gtWaves, 10, GUICallbackHandler);
		gwlbTWaves->add_item(0, "Directional");
		gwlbTWaves->add_item(1, "Circular");


	gwNWaves = glui->add_spinner("# Waves:",GLUI_SPINNER_INT, NULL, 20, GUICallbackHandler );
			gwNWaves->set_int_limits( 1, 8, GLUI_LIMIT_CLAMP );	
	glui->add_separator();

	char label[10];
	for (int i=0; i<count; i++) {
		
		if (i%4==0) glui->add_column();

		sprintf (label, "Wave #%ld", i+1);
		//glui->add_statictext (label);
		GLUI_Panel *tPanel = glui->add_panel( label );

		GLUI_Spinner *t1Spinner = glui->add_spinner_to_panel(tPanel, "Amplitude:",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t1Spinner->set_int_limits( -1, 1, GLUI_LIMIT_CLAMP );	
			gwAmpSpinnerCtrl[i] = t1Spinner;

		GLUI_Spinner *t4Spinner = glui->add_spinner_to_panel(tPanel, "Speed: ",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t4Spinner->set_int_limits( -1, 1, GLUI_LIMIT_CLAMP );	
			gwSpeedSpinnerCtrl[i] = t4Spinner;

		GLUI_Spinner *t2Spinner = glui->add_spinner_to_panel(tPanel, "Direction X:",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t2Spinner->set_int_limits( -1, 1, GLUI_LIMIT_CLAMP );	
			gwDxSpinnerCtrl[i] = t2Spinner;

		GLUI_Spinner *t3Spinner = glui->add_spinner_to_panel(tPanel, "Direction Y:",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t3Spinner->set_int_limits( -1, 1, GLUI_LIMIT_CLAMP );	
			gwDySpinnerCtrl[i] = t3Spinner;

		GLUI_Spinner *t5Spinner = glui->add_spinner_to_panel(tPanel, "Wave Length:",GLUI_SPINNER_FLOAT, NULL, 30, GUICallbackHandler );
			t5Spinner->set_int_limits( -1, 1, GLUI_LIMIT_CLAMP );	
			gwWlSpinnerCtrl[i] = t5Spinner;
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
	
	// glc: sync params
	syncVal();

	glutMainLoop();

	return (TRUE);
}
