#ifndef __SCENE3D_H__
#define __SCENE3D_H__

#include "sdk_swak.h"

#include "Importer.hpp"
#include "PostProcess.h"
#include "Scene.h"

class Scene3D {
	public:
		Scene3D(const char*);
		~Scene3D();

		GLuint getVAO(int);
		GLsizei getIndexCount(int) const;

	private:
		bool init();
		bool initGL();

	private:
		const char*				_filePath;
		std::vector<GLuint>		_vao;
		bool					_doneInitGL;
		int						_meshCount;

		//bounding box coords
		std::vector<aiVector3D>	_min;
		std::vector<aiVector3D>	_max;

		//vbo for positions
		std::vector<GLuint>		_VBOpositions;

		//vbo for positions
		std::vector<GLuint>		_VBOnormals;
		
		//vbo for positions
		std::vector<GLuint>		_VBOtexCoords;
		
		//vbo for indices
		std::vector<GLuint>		_VBOindices;
		std::vector<GLsizei>	_indexCount;
};

#endif