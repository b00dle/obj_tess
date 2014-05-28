#version 430

layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec2 tcTexCoord[];
out vec3 tePosition;
out vec3 tePatchDistance;
out vec2 teTexCoord;

uniform mat4 ModelViewProjection;
uniform mat4 ModelView;

uniform sampler2D tex;

void main(){
	vec3 p0 = gl_TessCoord.x * tcPosition[0];
	vec3 p1 = gl_TessCoord.y * tcPosition[1];
	vec3 p2 = gl_TessCoord.z * tcPosition[2];
	vec3 pos = p0 + p1 + p2;

	vec2 tc0 = gl_TessCoord.x * tcTexCoord[0];
	vec2 tc1 = gl_TessCoord.y * tcTexCoord[1];
	vec2 tc2 = gl_TessCoord.z * tcTexCoord[2];	
	teTexCoord = tc0 + tc1 + tc2;

	vec3 normal = normalize(cross(tcPosition[1] - tcPosition[0], 
								  tcPosition[2] - tcPosition[0]));

	float height = texture(tex, teTexCoord).x;
	height = height - 1.0f;
	pos += (normal * height);

	gl_Position = ModelViewProjection * vec4(pos, 1);
	tePosition = vec3(ModelView * vec4(pos,1.0)).xyz;
	tePatchDistance = gl_TessCoord;
}