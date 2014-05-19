#include "icosahedron.hpp"
#include <iostream>

Icosahedron::Icosahedron():
	_faces			(),
	_verts			(),
	_vao			(0),
	_VBOpositions	(0),
	_stride			(0),
	_positionSlot	(0),
	_VBOindices		(0),
	_indexCount		(0),
	_doneInitGL		(false)
{
	init();
}

Icosahedron::~Icosahedron()
{}

void Icosahedron::init() {
	const float verts[] = {
         0.000f,  0.000f,  1.000f,
         0.894f,  0.000f,  0.447f,
         0.276f,  0.851f,  0.447f,

        -0.724f,  0.526f,  0.447f,
        -0.724f, -0.526f,  0.447f,
         0.276f, -0.851f,  0.447f,
         
		 0.724f,  0.526f, -0.447f,
        -0.276f,  0.851f, -0.447f,
        -0.894f,  0.000f, -0.447f,
        
		-0.276f, -0.851f, -0.447f,
         0.724f, -0.526f, -0.447f,
         0.000f,  0.000f, -1.000f 
	};

	const int faces[] = {
		2, 1, 0,
		3, 2, 0,
		4, 3, 0,
		5, 4, 0,
		1, 5, 0,
		
		11, 6,  7,
		11, 7,  8,
		11, 8,  9,
		11, 9,  10,
		11, 10, 6,
		
		1, 2, 6,
		2, 3, 7,
		3, 4, 8,
		4, 5, 9,
		5, 1, 10,
		
		2,  7, 6,
		3,  8, 7,
		4,  9, 8,
		5, 10, 9,
		1, 6, 10
	};
	
	for(int i = 0; i < 60; ++i){
		_faces[i] = faces[i];
	}
	for(int i = 0; i < 36; ++i){
		_verts[i] = verts[i];
	}

	_indexCount = sizeof(_faces) / sizeof(_faces[0]);
	_stride = 3 * sizeof(float);
}

void Icosahedron::initGL() {
	//create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//create VBO for positions
	glGenBuffers(1, &_VBOpositions);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOpositions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_verts), _verts, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, _stride, 0);
	
	//create VBO for indices
	glGenBuffers(1, &_VBOindices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBOindices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_faces), _faces, GL_STATIC_DRAW);

	glBindVertexArray(0); //unbind vertex array

	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

	_doneInitGL = true;
}

GLuint Icosahedron::getVAO() {
	if(!_doneInitGL)
		initGL();
	return _vao;
}

GLsizei Icosahedron::getIndexCount() const {
	return _indexCount;
}
