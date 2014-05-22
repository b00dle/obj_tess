#version 430

layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
out vec3 tePosition;
out vec3 tePatchDistance;

uniform mat4 ModelViewProjection;
uniform mat4 ModelView;

void main(){
	vec3 p0 = gl_TessCoord.x * tcPosition[0];
	vec3 p1 = gl_TessCoord.y * tcPosition[1];
	vec3 p2 = gl_TessCoord.z * tcPosition[2];
	vec3 pos = normalize(p0 + p1 + p2);


	gl_Position = ModelViewProjection * vec4(pos, 1);
	tePosition = vec3(ModelView * vec4(pos,1.0)).xyz;
	tePatchDistance = gl_TessCoord;
}