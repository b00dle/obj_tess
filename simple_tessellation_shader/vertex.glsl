#version 430

layout(location = 0) in vec4 vertex;
layout(location = 2) in vec2 texCoord;

out vec3 vPosition;
out vec2 vTexCoord;

//uniform mat4 ModelViewProjection;

void main() {
	vPosition = vertex.xyz;
	vTexCoord = texCoord;
	//gl_Position = ModelViewProjection * vertex;
}