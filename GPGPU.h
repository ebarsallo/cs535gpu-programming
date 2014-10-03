#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

#include <ctime>

// Params
static GLfloat sinParamAmplitude[] = {1, 0.8, 0.6, 0.75, 0.45, 0.8, 0.6, 0.75, 1, 0.8};
static GLfloat sinParamDx[] = {1, 1, 0.5, -0.5, 0, 0.1, 0.5, -0.5, 1, 1};
static GLfloat sinParamDy[] = {0, 0.1, 0.5, -0.5, 1, 1, 0.5, -0.5, 0, 0.1};
static GLfloat sinParamWaveLength[] = {0.05, 0.075, 0.05, 0.02, 0.01, 0.003, 0.04, 0.05, 0.075, 0.05};
static GLfloat sinParamSpeed[] = {0.01, 0.025, 0.025, 0.01, 0.025, 0.01, 0.03, 0.008, 0.01, 0.025};

// Circular Waves
static GLfloat sinParamCenterX[] = {0.3, 0.7, 0.9, 0.5, 0.3, 0.7, 0.5, 0.8, 0.5, 0.3};
static GLfloat sinParamCenterY[] = {0.2, 0.4, 0.8, 0.5, 0.2, 0.4, 0.2, 0.3, 0.5, 0.2};

static GLint sinNWaves = 3;
static GLint sinTWaves = 0;	// Default: directional

// Islands
struct strIsland{
	GLfloat xy[4];
	GLfloat height[4];
	GLfloat area[4];
};
typedef strIsland t_island;

static t_island island[4];



class GPGPU
{
public:
	GPGPU(int w, int h);

	void restart();
	void update();
	void display();
	int loadShader(char* filename, std::string& text);

	// gpu
	void updateSineParams(GLfloat* arr[]);

	// gpu: setter
	void setSinNWaves(int n);
	void setSinTWaves(int n);
	void setSinParamAmplitude(float arr[]);
	void setSinParamDx(float arr[]);
	void setSinParamDy(float arr[]);
	void setSinParamWaveLength(float arr[]);
	void setSinParamSpeed(float arr[]);
	void setSinParamCenterX(float arr[]);
	void setSinParamCenterY(float arr[]);


private:
    int _width;				// width of the screen
	int _height;			// height of the screen
	int _initialized;		// if the cells are initialized (=1) or not (=0)

	GLuint _initializedLoc;
    GLuint _texUnitLoc;
    
    GLuint _textureId;		// The texture ID used to store data array
    GLuint _programId;		// the program ID

    GLuint _fragmentShader;

	// gpu
	GLfloat _timeLoc;
	GLfloat _sinParamALoc;
	GLfloat _sinParamDxLoc;
	GLfloat _sinParamDyLoc;
	GLfloat _sinParamWlLoc;
	GLfloat _sinParamSpLoc;

	GLfloat _sinParamCxLoc;
	GLfloat _sinParamCyLoc;

	clock_t _ck_start;
	double _currenttime;

	GLuint _wSizeLoc;
	GLuint _wTypeLoc;


	void init();
	void setupIsland();
	void setupSinParams();

};

