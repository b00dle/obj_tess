#ifndef __OBJECT3D_H__
#define __OBJECT3D_H__

#include "sdk_swak.h"

#include "Importer.hpp"
#include "PostProcess.h"
#include "Scene.h"

class Object3D {
	public:
		Object3D(const char*);
		~Object3D();

		GLuint getVAO();
		GLsizei getIndexCount() const;

	private:
		bool init();
		bool initGL();

	private:
		const char*		_filePath;
		GLuint			_vao;
		bool			_doneInitGL;

		//vbo for positions
		GLuint			_VBOpositions;

		//vbo for positions
		GLuint			_VBOnormals;
		
		//vbo for positions
		GLuint			_VBOtexCoords;
		
		//vbo for indices
		GLuint			_VBOindices;
		GLsizei			_indexCount;
};

#endif