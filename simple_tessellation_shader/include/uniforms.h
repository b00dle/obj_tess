
#ifndef CPP

#define sdk_bool bool

layout(std140, binding=1) uniform
#else
struct
#endif
TessellationParams
{
	float innerTessFactor; //number of nested primitives
	float outerTessFactor; //number of times to subdivide each edge
	float distance;
	float df;

	SDK_MAT4 ModelView;
	SDK_MAT4 ModelViewProjection;

	SDK_BOOL light;
	SDK_VEC3 lightPosition;
	
	SDK_BOOL lod;
	SDK_BOOL dMapping;
	SDK_BOOL nMapping;
	SDK_BOOL textured;
	SDK_BOOL triangles;

#ifdef CPP
	TessellationParams() :
		innerTessFactor(16.0f),
		outerTessFactor(16.0f),
		distance(4.0f),
		light(true),
		lightPosition( 0.25f, 0.25f, 1.0f),
		df(0.2f),
		lod(true),
		dMapping(true),
		nMapping(true),
		textured(true),
		triangles(false)
		{}
#endif
};
