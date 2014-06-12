#version 430

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec2 texCoord;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;

void main() {
	vPosition = vertex.xyz;
	vNormal = normal.xyz;
	vTexCoord = texCoord;
}