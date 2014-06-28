#version 430
#extension GL_EXT_texture_array : enable

layout(location = 0) out vec4 fragColor;

in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec2 gTexCoord;
in vec3 gPosition;
in float gAge;

uniform float lit;
uniform float nMapping;
uniform float textured;
uniform float triangles;
uniform vec3 light;

uniform sampler2D texColorYoung;
uniform sampler2D texNormalYoung;

uniform sampler2D texColorMid;
uniform sampler2D texNormalMid;

uniform sampler2D texColorOld;
uniform sampler2D texNormalOld;

uniform sampler2DArray texArray;

float amplify(float d, float scale, float offset) {
	d = scale * d + offset;
	d = clamp(d, 0, 1);
	d = 1 -exp2(-2*d*d);
	return d;
}

mat3 computeTextureSpaceMatrix(void) {
	//derivations of world position
	vec3 pos_dx = dFdx(gPosition);
	vec3 pos_dy = dFdy(gPosition);

	//derivations of texture coordinates
	vec2 tex_dx = dFdx(gTexCoord);
	vec2 tex_dy = dFdy(gTexCoord);

	//initial tangent & bitangent vectors
	vec3 t = normalize( tex_dy.y * pos_dx - tex_dx.y * pos_dy );
	vec3 b = normalize( tex_dy.x * pos_dx - tex_dx.x * pos_dy );

	//generate new tangent from input normal
	vec3 n = normalize(gFacetNormal);
	vec3 x = cross(n, t);
	t = cross(x, n);
	t = normalize(t);

	//update bitangent
	x = cross(b, n);
	b = cross(n, x);
	b = normalize(b);

	return mat3(t, b, n); //TextureSpaceMatrix
}

vec3 computeTexValue(bool normalTex) {
/*
	tf = transition factor (for interpolating between bark ages)
*/
	float tf = 0.0;
	if(gAge < 0.25)
		return normalTex ? texture(texNormalYoung, gTexCoord).rgb * 2.0 - 1.0 : texture(texColorYoung, gTexCoord).rgb;
	
	else if(gAge < 0.75) {
		tf = min( (gAge-0.25) / 0.25 , 1.0 ); //only influences a premature midgrowth stage 0.25 < age < 0.5
		return normalTex ? (tf * texture(texNormalMid, gTexCoord).rgb + (1-tf) * texture(texNormalYoung, gTexCoord).rgb) * 2.0 - 1.0 :
							tf * texture(texColorMid, gTexCoord).rgb + (1-tf) * texture(texColorYoung, gTexCoord).rgb;
	}
	else {
		tf = min( (gAge-0.75) / 0.25 , 1.0 );
		return normalTex ? (tf * texture(texNormalOld, gTexCoord).rgb + (1-tf) * texture(texNormalMid, gTexCoord).rgb) * 2.0 - 1.0 :
							tf * texture(texColorOld, gTexCoord).rgb + (1-tf) * texture(texColorMid, gTexCoord).rgb;
	}
}

void main() {
	#ifndef ORANGE_PURPLE
		vec3 color = gl_FrontFacing ? vec3(0.8,0.1,0.0) : vec3(0.0,0.0,1.0);
	#else
		vec3 color = gl_FrontFacing ? vec3(1.0,0.6,0.0) : vec3(0.6,0.0,1.0);
	#endif
	if (lit > 0.5) {
		///// compute light intensity factors /////	
		vec3 N = normalize(gFacetNormal);

		if(nMapping > 0.5) {
			mat3 texSpaceMat = computeTextureSpaceMatrix();
			vec3 normalLookup = computeTexValue(true); 
			N = texSpaceMat * normalize(normalLookup);
			N = normalize(N + gFacetNormal);
		}

		vec3 L = normalize(light);
		float df = abs(dot(N,L)); //diffuse factor

		///// compute color of material /////		
		if(textured > 0.5) {
			color = computeTexValue(false);
			//color = texture2DArray(texArray, vec3(gTexCoord, 0)).xyz;
		}
		
		color = df * color;

		///// show triangle edges /////
		if(triangles > 0.5) {
			float d1 = min(min(gTriDistance.x, gTriDistance.y), gTriDistance.z);
			float d2 = min(min(gPatchDistance.x, gPatchDistance.y), gPatchDistance.z);
			color = amplify(d1, 40, -0.5) * amplify(d2, 60, -0.5) * color;
		}
		
		///// set output fragment color /////
		fragColor = vec4(color,1.0);
	}
	else {
		fragColor = vec4(color,1.0);
	}
}
