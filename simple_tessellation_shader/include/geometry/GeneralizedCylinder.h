#ifndef __GENERALIZED__CYLINDER__H__
#define __GENERALIZED__CYLINDER__H__

#define GLM_SWIZZLE

#include "Bezierpath.h"
#include "Contour.h"
#include "Path.h"

#include "sdk_swak.h"

#include <vector>


class GeneralizedCylinder
{

public:

	//C'tor
	GeneralizedCylinder();
	GeneralizedCylinder(Path const&, Contour const&, std::vector<float> const&, std::vector<float> const&, std::vector<float> const&, int = 0);

	//Getter
	std::vector<nv::vec3f>*    vertices();	
	std::vector<nv::vec3f>*    normals();	
	std::vector<unsigned int>* indices();
	std::vector<float>*        color();

	GLuint getVAO();
	GLsizei getIndexCount() const;

private:

	void initGL();

	//private thickness function
	float                      getThickness(int);

	
	void                       calculateIndices();
	void                       calculateNormals();
	void                       calculateColors();
	void					   calculateTexCoords();

private:

	std::vector<float>         mRadii;
	std::vector<nv::vec3f>     mVertices;
	std::vector<nv::vec3f>     mNormals;
	std::vector<unsigned int>  mIndices;
	std::vector<float>         mColor;
	std::vector<nv::vec2f>     mTexCoords;
	std::vector<float>		   mAge;

	unsigned int               mOffset;

	Path                       mBezierPath;
	Contour                    mContour;

	//GL
	//vertex array object
	GLuint _vao;

	//vbo for positions
	GLuint _VBOpositions;

	//vbo for positions
	GLuint _VBOnormals;
		
	//vbo for positions
	GLuint _VBOtexCoords;
		
	//vbo for indices
	GLuint _VBOindices;
	GLsizei _indexCount;

	//vbo for ages
	GLuint _VBOage;

	bool _doneInitGL;
};


#endif