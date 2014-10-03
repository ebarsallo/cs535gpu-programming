#include "GPGPU.h"
#include <iostream>
#include <fstream>

//#include <windows.h>
//#include <float.h>

GPGPU::GPGPU(int w, int h) : _initialized(0), _width(w), _height(h)
{
    // Create a texture to store the framebuffer
    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGB, GL_FLOAT, 0);

    _programId = glCreateProgram();

	// Load fragment shader which will be used as computational kernel
	std::string edgeFragSource2;
	loadShader("fragment.glsl", edgeFragSource2);

    // Create the fragment program
    _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* source = edgeFragSource2.c_str();
    glShaderSource(_fragmentShader, 1, &source, NULL);
    glCompileShader(_fragmentShader);
    glAttachShader(_programId, _fragmentShader);

    // Link the shader into a complete GLSL program.
    glLinkProgram(_programId);

	// Check program
	{
		int infologLength = 0;
		glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &infologLength);
		if (infologLength > 0) {
			char *infoLog = (char *)malloc(infologLength);
			glGetProgramInfoLog(_programId, infologLength, NULL, infoLog);
			printf("%s\n",infoLog);
			free(infoLog);
		}
	}

    // Get location of the uniform variables
    _texUnitLoc = glGetUniformLocation(_programId, "texUnit");
	_initializedLoc = glGetUniformLocation(_programId, "initialized");

	// gpu
	_timeLoc = glGetUniformLocation(_programId, "time");	
	_ck_start = std::clock();
	_wSizeLoc = glGetUniformLocation(_programId, "wSize");
	_wTypeLoc = glGetUniformLocation(_programId, "wType");

	// gpu: sine params
	_sinParamALoc  = glGetUniformLocation(_programId, "gAi");
	_sinParamDxLoc = glGetUniformLocation(_programId, "gDx");
	_sinParamDyLoc = glGetUniformLocation(_programId, "gDy");
	_sinParamWlLoc = glGetUniformLocation(_programId, "gwl");
	_sinParamSpLoc = glGetUniformLocation(_programId, "gSp");

	_sinParamCxLoc = glGetUniformLocation(_programId, "gCx");
	_sinParamCyLoc = glGetUniformLocation(_programId, "gCy");

	// gpu: config islands
	setupIsland();


}

/**
 * <summary>Restart geometries.</summary>
 */
void GPGPU::restart()
{
	_initialized = 0;

	// gpu
	_ck_start = std::clock();
	setupSinParams();
	setupIsland();

	
}

/**
 * <summary>Assign a set of floats to an array.</summary>
 */
void 
setFloat (GLfloat *arr, float v1, float v2, float v3, float v4)
{
	arr[0] = v1;
	arr[1] = v2;
	arr[2] = v3;
	arr[3] = v4;
}

/**
 * <summary>Arrange island in the scene.
 * The window is divided in 4 imaginary quadrant. In order to avoid collision between island, each quadrant will have only
 * one island.</summary>
 */
void GPGPU::setupIsland()
{
	GLfloat px1, px2, px3, px4;
	GLfloat py1, py2, py3, py4;

	srand (time (NULL));	
	px1 = (float) (rand() % 35) / 100;			py1 = (float) (rand() % 35) / 100;
	px2 = 0.5f + (float) (rand() % 35) / 100;	py2 = (float) (rand() % 35) / 100;
	px3 = (float) (rand() % 35) / 100;			py3 = 0.5f + (float) (rand() % 35) / 100;
	px4 = 0.5f + (float) (rand() % 35) / 100;	py4 = 0.5f + (float) (rand() % 35) / 100;


	// Island #1
	island[0].xy[0] = px1;		island[0].xy[1] = py1;		island[0].xy[2] = 0.1f;
	setFloat (island[0].height, 1, 2, 5, 20);
	setFloat (island[0].area, 0.05, 0.05, 0.1, 0.8);

	// Island #2s
	island[1].xy[0] = px2;		island[1].xy[1] = py2;		island[1].xy[2] = 0.086f;
	setFloat (island[1].height, 1, 3, 5, 20);
	setFloat (island[1].area, 0.1, 0.1, 0.3, 0.5);

	// Island #3
	island[2].xy[0] = px3;		island[2].xy[1] = py3;		island[2].xy[2] = 0.125f;	
	setFloat (island[2].height, 1, 4, 8, 2);
	setFloat (island[2].area, 0.05, 0.05, 0.2, 0.7);

	// Island #4
	island[3].xy[0] = px4;		island[3].xy[1] = py4;		island[3].xy[2] = 0.06f;
	setFloat (island[3].height, 1, 3, 6, 20);
	setFloat (island[3].area, 0.05, 0.1, 0.25, 0.6);
}

/**
 * <summary>Communicate the sines params to the shader.</summary>
 */
void 
GPGPU::setupSinParams()
{
	// Sine params
	glUniform1fv(_sinParamALoc,  10, sinParamAmplitude);
	glUniform1fv(_sinParamDxLoc, 10, sinParamDx);
	glUniform1fv(_sinParamDyLoc, 10, sinParamDy);
	glUniform1fv(_sinParamWlLoc, 10, sinParamWaveLength);
	glUniform1fv(_sinParamSpLoc, 10, sinParamSpeed);

	glUniform1fv(_sinParamCxLoc, 10, sinParamCenterX);
	glUniform1fv(_sinParamCyLoc, 10, sinParamCenterY);

	glUniform1i(_wSizeLoc, sinNWaves);
	glUniform1i(_wTypeLoc, sinTWaves);

	GLfloat myLoc;
	char fmt[20];
	for (int i=0; i<4; i++) {
		sprintf (fmt, "gIsland[%ld].xy", i);		myLoc = glGetUniformLocation(_programId, fmt);	glUniform1fv(myLoc, 4, island[i].xy);
		sprintf (fmt, "gIsland[%ld].height", i);	myLoc = glGetUniformLocation(_programId, fmt);	glUniform1fv(myLoc, 4, island[i].height);
		sprintf (fmt, "gIsland[%ld].area", i);		myLoc = glGetUniformLocation(_programId, fmt);	glUniform1fv(myLoc, 4, island[i].area);
	}

}

/**
 * <summary>Update all sines params used to simulated water</summary>
 */
void 
updateSineParams(GLfloat* arr[]) 
{
}


/**
 * <summary>Setter. Set number of waves.</summary>
 */
void 
GPGPU::setSinNWaves(int n)
{
	sinNWaves = n;
}

/**
 * <summary>Setter. Set number of waves.</summary>
 */
void 
GPGPU::setSinTWaves(int n)
{
	sinTWaves = n;
}

/**
 * <summary>Setter. Set altitude of the wave.</summary>
 */
void 
GPGPU::setSinParamAmplitude(float arr[])
{
	for (int i=0; i<sinNWaves; i++)
		sinParamAmplitude[i] = arr[i];
}

/**
 * <summary>Setter. Set altitude of the wave.</summary>
 */
void 
GPGPU::setSinParamDx(float arr[])
{
	for (int i=0; i<sinNWaves; i++)
		sinParamDx[i] = arr[i];
}

/**
 * <summary>Setter. Set altitude of the wave.</summary>
 */
void 
GPGPU::setSinParamDy(float arr[])
{
	for (int i=0; i<sinNWaves; i++)
		sinParamDy[i] = arr[i];
}

/**
 * <summary>Setter. Set altitude of the wave.</summary>
 */
void 
GPGPU::setSinParamWaveLength(float arr[])
{
	for (int i=0; i<sinNWaves; i++)
		sinParamWaveLength[i] = arr[i];
}

/**
 * <summary>Setter. Set altitude of the wave.</summary>
 */
void 
GPGPU::setSinParamSpeed(float arr[])
{
	for (int i=0; i<sinNWaves; i++)
		sinParamSpeed[i] = arr[i];
}

/**
 * <summary>update</summary>
 */
void GPGPU::update()
{
    // Backup the viewport dimensions
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(0, 0, _width, _height);
        
    // Copy the results to the texture
	glBindTexture(GL_TEXTURE_2D, _textureId);

	glUseProgram(_programId);
            
    // Set the uniform variables
    glUniform1i(_texUnitLoc, 0);
	glUniform1i(_initializedLoc, _initialized);
	_initialized = 1;

	// gpu: <begin>
	// currenttime
	_currenttime = double(std::clock() - _ck_start ) / (double)(CLOCKS_PER_SEC/1000);
	glUniform1f(_timeLoc, _currenttime);

	setupSinParams();
	// gpu: <end>

	// By drawing a quad, the fragment shader will be called for each pixel.
    glBegin(GL_QUADS);
    {            
        glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.5f);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.5f);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.5f);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.5f);
    }
    glEnd();

    glUseProgram(0);
        
    // Copy the rendered image to the texture so that we can start from the current state in the next iteration
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _width, _height);
        
    // Restore the stored viewport dimensions
	glViewport(vp[0], vp[1], vp[2], vp[3]);
}

void GPGPU::display()
{
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glEnable(GL_TEXTURE_2D);

    // Render the result image.
    glBegin(GL_QUADS);
    {
        glTexCoord2f(0, 0); glVertex3f(-1, -1, -0.5f);
        glTexCoord2f(1, 0); glVertex3f( 1, -1, -0.5f);
        glTexCoord2f(1, 1); glVertex3f( 1,  1, -0.5f);
        glTexCoord2f(0, 1); glVertex3f(-1,  1, -0.5f);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

int GPGPU::loadShader(char* filename, std::string& text)
{
	std::ifstream ifs;
	ifs.open(filename, std::ios::in);

	std::string line;
	while (ifs.good()) {
        getline(ifs, line);

		text += line + "\n";
    }

	return 0;
}