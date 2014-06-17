#version 430

layout(location = 0) out vec4 fragColor;

in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;
in vec2 gTexCoord;
in vec3 gPosition;

uniform float lit;
uniform float nMapping;
uniform float textured;
uniform float triangles;
uniform vec3 light;
uniform sampler2D texColor;
uniform sampler2D texNormal;

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
			N = texSpaceMat * normalize( texture(texNormal, gTexCoord).rgb * 2.0 - 1.0 );
			N = normalize(N + gFacetNormal);
		}

		vec3 L = light;
		float df = abs(dot(N,L));

		///// compute color of material /////		
		if(textured > 0.5)
			color = texture(texColor, gTexCoord).xyz; 
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
