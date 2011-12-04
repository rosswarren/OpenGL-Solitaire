#version 120 // min version of shader needed

// Metaball shader for the background

uniform float time;
uniform vec2 screenSize;

void main(void) {
	// setup the world coordinates vector
	vec2 world = -1.0 + 2.0 * gl_FragCoord.xy / screenSize.xy;

	// array of positions on the sin and cos waves to be calculated into colour and light values
	vec2 [4] wavePositions;

	// dot products of calculations with the screen to get a value appropriate to the position and screen size
	float[4] dotProducts;

	// setup the positions for up/down and left/right movement 
	wavePositions[0].x = cos(time * 0.5); // first value is used for background colour also
	wavePositions[0].y = sin(time * 0.2);
	wavePositions[1].x = cos(time * 0.3);
	wavePositions[1].y = sin(time * 0.4);
	wavePositions[2].x = sin(time * 0.7);
	wavePositions[2].y = cos(time * 0.9);
	wavePositions[3].x = sin(time * 0.8);
	wavePositions[3].y = cos(time * 0.1);

	// setup the values properly mapped to world coordinates
	dotProducts[0] = dot(world - wavePositions[0], world - wavePositions[0]) * 18.0;
	dotProducts[1] = dot(world + wavePositions[1], world + wavePositions[1]) * 12.0;
	dotProducts[2] = dot(world + wavePositions[2], world + wavePositions[2]) * 14.0;
	dotProducts[3] = dot(world + wavePositions[3], world + wavePositions[3]) * 16.0;

	// the light value at the position
	float light = 0;

	// calculate the light value 
	for (int i = 0; i < 4; i++) {
		light += ((float(i + 6.0)) / 10.0) / dotProducts[i];
	}

	float x = light;
	float y = pow(light, 1.0 + wavePositions[0].x / 1.5);
	float z = y;

	gl_FragColor = vec4(x, y, z, 1.0);
}