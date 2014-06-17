#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 ModelView;

in vec3 tePosition[3];
in vec3 tePatchDistance[3];
in vec2 teTexCoord[3];
in vec3 teNormal[3];
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;
out vec2 gTexCoord;
out vec3 gPosition;

void main() {
	
	gFacetNormal = teNormal[0];
	gPatchDistance = tePatchDistance[0];
	gTexCoord = teTexCoord[0];
	gTriDistance = vec3(1,0,0);
	gl_Position = gl_in[0].gl_Position;
	gPosition = gl_Position.xyz;
	EmitVertex();
	
	gFacetNormal = teNormal[1];
	gPatchDistance = tePatchDistance[1];
	gTexCoord = teTexCoord[1];
	gTriDistance = vec3(0,1,0);
	gl_Position = gl_in[1].gl_Position;
	gPosition = gl_Position.xyz;
	EmitVertex();
	
	gFacetNormal = teNormal[2];
	gPatchDistance = tePatchDistance[2];
	gTexCoord = teTexCoord[2];
	gTriDistance = vec3(0,0,1);
	gl_Position = gl_in[2].gl_Position;
	gPosition = gl_Position.xyz;
	EmitVertex();

	EndPrimitive();
}

