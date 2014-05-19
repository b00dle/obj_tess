#version 430

layout(vertices = 3) out;

in vec3 vPosition[];
out vec3 tcPosition[];

uniform float innerTessLevel;
uniform float outerTessLevel;

uniform float lod;
uniform float distance;

void main(){
	
	float inTess  = innerTessLevel;
	float outTess = outerTessLevel;

	if(lod > 0.5){
		inTess  *=  2.0f/sqrt(distance);
		outTess *= 2.0f/sqrt(distance);
	}

	tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
	if(gl_InvocationID == 0) {
		gl_TessLevelInner[0] = inTess;
        gl_TessLevelOuter[0] = outTess;
        gl_TessLevelOuter[1] = outTess;
        gl_TessLevelOuter[2] = outTess;
	}
}