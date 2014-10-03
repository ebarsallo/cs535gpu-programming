uniform int initialized;
uniform sampler2D texUnit;

uniform float time;

const float PI = 3.14159;

// Sine params
uniform float gAi[10];
uniform float gDx[10];
uniform float gDy[10];
uniform float gwl[10];
uniform float gSp[10];
uniform float gCx[10];
uniform float gCy[10];

uniform int wSize;	// Number of waves
uniform int wType;	// Type of Wave (0: Directional, 1: Circular)

// Island
struct strIsland{
	float xy[4];
	float height[4];
	float area[4];
};
uniform strIsland gIsland[4];

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
 * Checks if a number (float) is between a range (min:a & max:b)
 */
bool between(float x, float a, float b)
{
	if (x >= a && x <= b)
		return true;
	return false;
}

/**
 * <summary>Checks if a point (x,y) is inside a circle.</summary>
 */
bool insideCircle(float x, float y, float cx, float cy, float r)
{
	float d = sqrt (pow(cx-x, 2) + pow(cy-y, 2));
	return d <= r;
}

/**
 * <summary>Checks if a point (x,y) is inside a circle area delimited by two distance (radio).</summary>
 */
bool insideCircleArea(float x, float y, float cx, float cy, float r1, float r2)
{
	float d = sqrt (pow(cx-x, 2) + pow(cy-y, 2));
	return (d >= r1) && (d <= r2);
}

/**
 * <summary>Checks if a point (x,y) lies inside an circular shape island.</summary>
 */
bool insideIslandCirc(strIsland my, float x, float y, float height, out int level)
{
	float cx, cy, h, r;
	bool bflag = false;

	level = -1;
	
	cx = my.xy[0];	r = my.xy[2];
	cy = my.xy[1];
	
	// Point (x,y) does not lies in the island
	if (! (insideCircle (x, y, cx, cy, r)))
		return false;

	// Verify if the wave height is over the height of the land.
	float p = 1; 
	float r1, r2 = r;
	for (int i=0; i<4; i++) {

		h  = my.height[i];
		p -= my.area[i];
		r1 = r * p;
		
		bflag = (bflag || (h > height));

		level = i;
		// check if the point is inside the portion of the land
		if (insideCircleArea (x, y, cx, cy, r1, r2)) {
			return bflag;
		} 
		r2 = r1;
	}
	
	return false;
}

/**
 * Checks if a point (x,y) lies inside an quadrangular shape island.
 */
bool insideIslandQuad(strIsland my, float x, float y, float height, out int level)
{
	float a, b, c, d, h;
	float sx, sy;
	
	level = -1;
	
	a = my.xy[0];	b = my.xy[2];
	c = my.xy[1];	d = my.xy[3];

	sx = b - a;
	sy = d - c;
	
	// Point (x,y) does not lies in the island
	if ( !(between (x, a, b) && 
		   between (y, c, d)) )
		return false;

	// Verify if the wave height is over the height of the land.
	float p = 0; 
	for (int i=3; i>=0; i--) {

		float nx, ny;
		float na, nb, nc, nd;

		p += my.area[i];
		nx = sx * p;
		ny = sy * p;
		
		na = a + sx/2.0 - nx/2.0;	nb = na + nx;
		nc = c + sy/2.0 - ny/2.0;	nd = nc + ny;
		h  = my.height[i];
		
		level = i;
		// check if the point is inside the portion of the land
		if (between (x, na, nb) && 
			between (y, nc, nd) &&
			(h > height)) {
			return true;
		} 
	}

	return false;
}

/**
 * mapColor
 * Assign a color to the pixel depending of the defining constraint:
 *  (1) Is the pixel inside an island?
 */
vec4 mapColor(float x, float y, float height)
{
	strIsland myIsland;
	vec4 color;
	int  lw;
	
	// Check if the point(x,y) lies inside any of the 4 island
	// island #1
	myIsland = gIsland[0];
	if (insideIslandCirc(myIsland, x, y, height, lw))
			return vec4(1, 0.70-lw*0.11, 0.0, 1.0);

	// island #2
	myIsland = gIsland[1];
	if (insideIslandCirc(myIsland, x, y, height, lw))
			return vec4(1, 0.70-lw*0.11, 0.0, 1.0);
	
	// island #3
	myIsland = gIsland[2];
	if (insideIslandCirc(myIsland, x, y, height, lw))
			return vec4(1, 0.70-lw*0.11, 0.0, 1.0);

	// island #4
	myIsland = gIsland[3];
	if (insideIslandCirc(myIsland, x, y, height, lw))
			return vec4(1, 0.70-lw*0.11, 0.0, 1.0);
	
	// shore of the island
	if (lw == 0) 
		color = vec4(0.9, 0.7+height*0.05, 0.9, 1.0);
	else
		color = vec4(0.0, height*0.25, 1.0, 1.0);

	return color;
}

/**
 * Water simulation using sine waves
 */
float sineWave(int i, vec2 vxy)
{
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

	// (x,y)
	float u = vxy.x;
	float v = vxy.y;

	if (wType == 0) {
		// Directional Wave
		theta = Dx*u + Dy*v;
	} else {
		// Circular Wave Test
		float du = u-gCx[i];
		float dv = v-gCy[i];
		theta = dot (vec2(-dv, du), vec2(dv, -du));
	}

	h = Ai * sin(theta * w + time * phi);
	
	// normalize the height such that start from 0, instead from -Ai.
	return h + Ai;
}

/**
 * Sum of sines.
 * Call 'n' times the sine wave equation to simulate a wave.
 */
void sumOfSinesWave()
{
	float height=0.0;
	vec4 color;

	for (int i=0;i<wSize;i++) {
		height += sineWave(i, gl_TexCoord[0].xy) / 2;
	}

	// update the color of this pixel
	color = mapColor(gl_TexCoord[0].xy.x, gl_TexCoord[0].xy.y, height);
	gl_FragColor = color;
}


void main()
{
	if (initialized == 0) {
		init();
	} else {
		sumOfSinesWave();
	}
}