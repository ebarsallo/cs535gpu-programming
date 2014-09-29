#pragma once

#define GLEW_STATIC 1
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>

#include <ctime>

// Params
static GLfloat sinParamAmplitude[] = {1, 0.8, 0.6, 0.75};
static GLfloat sinParamDx[] = {1.75, 1.75, 1.25, 1};
static GLfloat sinParamDy[] = {1.75, 1.75, 1.25, 1};
static GLfloat sinParamWaveLength[] = {2, 1, 0.5, 1};

class GPGPU
{
public:
	GPGPU(int w, int h);

	void restart();
	void update();
	void display();
	int loadShader(char* filename, std::string& text);

	// gpu
	void init();

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
	GLfloat _zerotimeLoc;
	GLfloat _sinParamALoc;
	GLfloat _sinParamDxLoc;
	GLfloat _sinParamDyLoc;
	GLfloat _sinParamWlLoc;

	clock_t _ck_start;
	double _currenttime;
};

