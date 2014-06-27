#version 430

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float age;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out float vAge;

void main() {
	vPosition	= vertex.xyz;
	vNormal		= normal.xyz;
	vTexCoord	= texCoord;
	vAge		= age - 0.4;
}