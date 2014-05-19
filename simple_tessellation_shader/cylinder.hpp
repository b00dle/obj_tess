#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include "sdk_swak.h"

class Cylinder {
	public:
		Cylinder();
		~Cylinder();

		GLuint getVAO(); /*const*/ //will trigger initGL if _doneInitGL == false
		GLsizei getIndexCount() const;

	private:
		void init();
		void initGL();

	private:
		int _faces[48];
		float _verts[48];

		//vertex array object
		GLuint _vao;

		//vbo for positions
		GLuint _VBOpositions;
		GLsizei _stride;
		GLuint _positionSlot;

		//vbo for indices
		GLuint _VBOindices;
		GLsizei _indexCount;

		bool _doneInitGL;
};

#endif