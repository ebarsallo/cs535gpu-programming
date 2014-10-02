#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

#include <ctime>

// Params
static GLfloat sinParamAmplitude[] = {1, 0.8, 0.6, 0.75};
static GLfloat sinParamDx[] = {1, 1, 0.5, -0.5};
static GLfloat sinParamDy[] = {0, 0.1, 0.5, -0.5};
static GLfloat sinParamWaveLength[] = {0.05, 0.075, 0.05, 0.02};
static GLfloat sinParamSpeed[] = {0.01, 0.025, 0.025, 0.01};

static GLint sinNWaves = 4;

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
	void setSinParamAmplitude(float arr[]);
	void setSinParamDx(float arr[]);
	void setSinParamDy(float arr[]);
	void setSinParamWaveLength(float arr[]);
	void setSinParamSpeed(float arr[]);


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

	clock_t _ck_start;
	double _currenttime;

	GLuint _wSizeLoc;


	void init();
	void setupIsland();
	void setupSinParams();

};

