#version 430

layout(vertices = 3) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec2 vTexCoord[];
out vec3 tcPosition[];
out vec3 tcNormal[];
out vec2 tcTexCoord[];

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
	tcNormal[gl_InvocationID]   = vNormal[gl_InvocationID];
	tcTexCoord[gl_InvocationID] = vTexCoord[gl_InvocationID];
	if(gl_InvocationID == 0) {
		gl_TessLevelInner[0] = inTess;
		gl_TessLevelInner[1] = inTess;
        gl_TessLevelOuter[0] = outTess;
        gl_TessLevelOuter[1] = outTess;
        gl_TessLevelOuter[2] = outTess;
		gl_TessLevelOuter[3] = outTess;
	}
}