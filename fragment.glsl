uniform int initialized;
uniform sampler2D texUnit;

uniform float time;

//const float PI = 3.14159265358979323846264;
const float PI = 3.1415;

// Sine params
uniform float gAi[10];
uniform float gDx[10];
uniform float gDy[10];
uniform float gwl[10];
uniform float gSp[10];

uniform int wSize;

/**
 * Initialize the image.
 * 
 * Use red component as U and blue component as V of Gray-Scott model.
 */
void init()
{
	vec2 texCoord = gl_TexCoord[0].xy;

	if (texCoord.x > 0.48 && texCoord.x < 0.52 && texCoord.y > 0.48 && texCoord.y < 0.52) {
		gl_FragColor = vec4(0.5, 0.0, 0.25, 1.0);	// purple
	} else {
		gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);	// red
	}
}


/**
 * Update the color of each pixel.
 *
 * Use Gray-Scott reaction-diffusion model to update U and V,
 * and U and V are stored in red and blue components of the pixel color, respectively.
 */
void grayScottReact()
{
	const float offset = 1.0 / 512.0;
	
	vec2 texCoord = gl_TexCoord[0].xy;
	
	// parameters for Gray-Scott model
	float F = 0.037;
	float K = 0.06;
	float Du = 0.209;
	float Dv = 0.102;

	// get the colors of the pixels
	vec2 c = texture2D(texUnit, texCoord).rb;
	vec2 l = texture2D(texUnit, texCoord + vec2(-offset, 0.0)).rb;
	vec2 t = texture2D(texUnit, texCoord + vec2(0.0, offset)).rb;
	vec2 r = texture2D(texUnit, texCoord + vec2(offset, 0.0)).rb;
	vec2 b = texture2D(texUnit, texCoord + vec2(0.0, -offset)).rb;

	float U = c.x;
	float V = c.y;
	vec2 lap = l + t + r + b  - c * 4.0;

	float dU = Du * lap.x - U * V * V + F * (1.0 - U);
	float dV = Dv * lap.y + U * V * V - (F + K) * V;

	// use the heat equation to updte the color of this pixel
	gl_FragColor = vec4(U + dU, 0.0, V + dV, 1.0);
}

/**
 * Water simulation using sine waves
 */
float sineWave(int i, vec2 vxy)
{
	//vec2 texCoord = gl_TexCoord[0].xy;

	float Ai = gAi[i];	 // amplitude
	float Dx = gDx[i];	 // direction 
	float Dy = gDy[i];	 // direction 
	float l  = gwl[i];	 // wavelength
	float s  = gSp[i];	 // speed
	
	float w  = 2.0*PI/l; // frequency
	float t  = 1.0;		 // time

	float theta;		 // angle of vector form by x,y point
	float phi = s;		 // phase

	float h;			 // height of wave

	// get the colors of the pixels
	//vec2 c = texture2D(texUnit, texCoord).rb;
	//vec2 c = texture2D(texUnit, vxy).rb;

	float u = vxy.x;
	float v = vxy.y;

	//theta = dot(vec2(cos(Dx),sin(Dy)), vec2(U,V));
	//theta = Dx * Dy;

	//theta = dot(vec2(Dx,Dy), vec2(U,V));
	//theta = PI / 2;
	theta = Dx*u + Dy*v;

	h = Ai * sin(theta * w + time * phi) + Ai; 

	return h;
}

/**
 * Sum of sines.
 * Call 'n' times the sine wave equation to simulate a wave.
 */
void sumOfSinesWave()
{
	float height=0.0;

	for (int i=0;i<wSize;i++) {
		height += sineWave(i, gl_TexCoord[0].xy) / 2;
	}

	// update the color of this pixel
	gl_FragColor = vec4(0.0, height*0.25, height, 1.0);
}


void main()
{
	if (initialized == 0) {
		init();
	} else {
		sumOfSinesWave();
		//sineWave();
		//grayScottReact();
	}
}