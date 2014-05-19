#version 430

layout(location = 0) in vec4 vertex;

out vec3 vPosition;

//uniform mat4 ModelViewProjection;

void main() {
    vPosition   = vertex.xyz;
	//gl_Position = ModelViewProjection * vertex;
}