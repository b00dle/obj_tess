#ifndef OBJFORMATTER_HPP
#define OBJFORMATTER_HPP

#include "sdk_swak.h"
#include "GL/glew.h"
#include <vector>

class ObjFormatter {
	public:
		ObjFormatter(const char*);
		~ObjFormatter();

		void printInfo(const char*) const;

	private:
		bool readOBJ(const char*);
		bool writeOBJ(const char*);
		nv::vec3<float> my_cross(nv::vec3<float> const&, nv::vec3<float> const&) const;

	private:
		std::vector<float> _vertices;
		std::vector<float> _normals;
		std::vector<float> _texCoords;
		std::vector<int>  _elements;

		int _numVertices;
		int _numNormals;
		int _numTexCoords;
		int _numElements;
};

#endif