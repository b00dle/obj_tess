#version 430

layout(location = 0) out vec4 fragColor;

/*in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

uniform float lit;
uniform vec3 light;

float amplify(float d, float scale, float offset) {
	d = scale * d + offset;
	d = clamp(d, 0, 1);
	d = 1 -exp2(-2*d*d);
	return d;
}*/


void main() {
	/*#ifndef ORANGE_PURPLE
		vec3 color = gl_FrontFacing ? vec3(1.0,0.0,0.0) : vec3(0.0,0.0,1.0);
	#else
		vec3 color = gl_FrontFacing ? vec3(1.0,0.6,0.0) : vec3(0.6,0.0,1.0);
	#endif
	if (lit > 0.5) {
		vec3 N = normalize(gFacetNormal);
		vec3 L = light;
		float df = abs(dot(N,L));
		color = df * color;

		float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
		float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
		color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
		fragColor = vec4(color,1.0);
	}
	else {
		fragColor = vec4(color,1.0);
	}*/
	fragColor = vec4(1.0,0.0,0.0,1.0);
}
