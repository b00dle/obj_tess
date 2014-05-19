#ifndef ICOSAHEDRON_HPP
#define ICOSAHEDRON_HPP

#include "sdk_swak.h"

class Icosahedron {
	public:
		Icosahedron();
		~Icosahedron();

		GLuint getVAO(); /*const*/ //will trigger initGL if _doneInitGL == false
		GLsizei getIndexCount() const;

	private:
		void init();
		void initGL();

	private:
		int _faces[60];
		float _verts[36];

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