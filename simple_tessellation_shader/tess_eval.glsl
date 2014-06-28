#version 430

layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];
in vec2 tcTexCoord[];
in float tcAge[];
out vec3 tePosition;
out vec3 tePatchDistance;
out vec2 teTexCoord;
out vec3 teNormal;
out float teAge;

uniform mat4 ModelViewProjection;
uniform mat4 ModelView;
uniform float df;
uniform float dMapping;
	
uniform sampler2D texHeightYoung;
uniform sampler2D texHeightMid;
uniform sampler2D texHeightOld;

float computeHeight() {
/*
	dc = displacement coefficient (based on age and growth stage)
	df = fixed displacement factor (uniform param)
	tf = transition factor (for interpolating between bark ages)
*/
	float dc = 0.0;
	float tf = 0.0;
	if(teAge < 0.25) {
		dc = teAge / 0.25;
		return dc * 0.33 * df * texture(texHeightYoung, teTexCoord).x;
	}
	else if(teAge < 0.75) {
		dc = teAge / 0.75;
		tf = min( (teAge-0.25) / 0.25 , 1.0 ); //only influences a premature midgrowth stage 0.25 < age < 0.5
		return df * (dc * tf * 0.66 * texture(texHeightMid, teTexCoord).x + (1-tf) * 0.33 * texture(texHeightYoung, teTexCoord).x);
	}
	else {
		dc = min( (teAge/1.0) , 1.0 );
		tf = min( (teAge-0.75) / 0.25 , 1.0 );
		return df * (dc * tf * texture(texHeightOld, teTexCoord).x + (1-tf) * 0.66 * texture(texHeightMid, teTexCoord).x);
	}
}

void main(){
	vec3 p0 = gl_TessCoord.x * tcPosition[0];
	vec3 p1 = gl_TessCoord.y * tcPosition[1];
	vec3 p2 = gl_TessCoord.z * tcPosition[2];
	vec3 pos = p0 + p1 + p2;

	vec3 n0 = gl_TessCoord.x * tcNormal[0];
	vec3 n1 = gl_TessCoord.y * tcNormal[1];
	vec3 n2 = gl_TessCoord.z * tcNormal[2];
	vec3 normal = normalize(n0 + n1 + n2);

	vec2 tc0 = gl_TessCoord.x * tcTexCoord[0];
	vec2 tc1 = gl_TessCoord.y * tcTexCoord[1];
	vec2 tc2 = gl_TessCoord.z * tcTexCoord[2];	
	teTexCoord = tc0 + tc1 + tc2;

	float a0 = gl_TessCoord.x * tcAge[0];
	float a1 = gl_TessCoord.y * tcAge[1];
	float a2 = gl_TessCoord.z * tcAge[2];	
	teAge = a0 + a1 + a2;

	if(dMapping > 0.5){
		float height = computeHeight();
		pos += normal * height;
	}

	gl_Position = ModelViewProjection * vec4(pos, 1);
	teNormal	= vec3(ModelView * vec4(normal,0.0)).xyz;
	tePosition  = vec3(ModelView * vec4(pos,1.0)).xyz;
	tePatchDistance = gl_TessCoord;
}