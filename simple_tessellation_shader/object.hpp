#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "sdk_swak.h"
#include "GL/glew.h"
#include <vector>

class Object {
	public:
		Object(const char*);
		~Object();

		GLuint getVAO(); /*const*/ //will trigger initGL if _doneInitGL == false
		GLsizei getIndexCount() const;
		const char16_t getIndices() const;
		
		void printInfo(const char*) const;

	private:
		void init();
		void initGL();
		bool loadOBJ(const char*);
		nv::vec3<float> my_cross(nv::vec3<float> const&, nv::vec3<float> const&) const;

	private:
		std::vector<float> _vertices;
		std::vector<nv::vec3<float>> _normals;
		std::vector<int>  _elements;

		//vertex array object
		GLuint _vao;

		//vertex buffer objects
		GLuint _VBOvertices;
		GLuint _VBOnormals;
		GLuint _VBOindices;

		GLsizei _indexCount;

		bool _doneInitGL;

};

#endif