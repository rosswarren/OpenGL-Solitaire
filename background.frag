uniform float time;
uniform vec2 resolution;

void main(void) {
	vec2 [4] move;
	vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;

	move[0].x = cos(time * 0.5);
	move[0].y = sin(time * 0.2);

	move[1].x = cos(time * 0.3);
	move[1].y = sin(time * 0.4);
	
	move[2].x = sin(time * 0.7);
	move[2].y = cos(time * 0.9);
	
	move[3].x = sin(time * 0.8);
	move[3].y = cos(time * 0.1);

	float r1 = dot(p - move[0], p - move[0]) * 18.0;
	float r2 = dot(p + move[1], p + move[1]) * 12.0;
	float r3 = dot(p + move[2], p + move[2]) * 14.0;
	float r4 = dot(p + move[3], p + move[3]) * 16.0;
	
	float metaball = 1.0 / r1 + 0.5 / r2 + 0.5 / r3 + 1.0 / r4;

	float x = pow(metaball, 1.0 + move[0].x);
	float y = pow(metaball, 1.0);
	float z = y;

	gl_FragColor = vec4(x, y, z, 1.0);
}