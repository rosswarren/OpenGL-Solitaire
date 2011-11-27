uniform float time;
uniform vec2 resolution;
float size=0.9;

void main(void) {
	vec2 move1,move2,move3,move4;
	move1.x = cos(time*0.5)*size;
	move1.y = sin(time*0.2)*size;
	move2.x = cos(time*0.3)*size;
	move2.y = sin(time*0.4)*size;
	move3.x = sin(time*0.7)*size;
	move3.y = cos(time*0.9)*size;
	move4.x = sin(time*0.8)*size;
	move4.y = cos(time*0.1)*size;

	vec2 p = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
	float r1 =(dot(p-move1,p-move1))*18.0;
	float r2 =(dot(p+move2,p+move2))*12.0;
	float r3 =(dot(p+move3,p+move3))*14.0;
	float r4 =(dot(p+move4,p+move4))*16.0;
	float metaball =(1.0/r1+0.5/r2+0.5/r3+1.0/r4);

	gl_FragColor = vec4(pow(metaball,1.0+move1.x), pow(metaball,1.0), pow(metaball,1.0), 1.0);
}