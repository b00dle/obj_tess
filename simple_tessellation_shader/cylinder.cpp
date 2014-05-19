#include "cylinder.hpp"
#include <iostream>

Cylinder::Cylinder():
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

Cylinder::~Cylinder()
{}

void Cylinder::init() {
	const float verts[] = {
        -0.763401f, -0.800936f, 0.763401f, 
		0.763401f, -0.800936f, 0.763401f, 
		-0.763401f, -0.800936f, -0.763401f,

		0.763401f, -0.800936f, -0.763401f, 
		-1.01532f, -0.800936f, 0.0f, 
		0.0f, -0.800936f, 1.01532f, 
		
		1.01532f, -0.800936f, 0.0f, 
		0.0f, -0.800936f, -1.01532f, 
		-0.763401f, 0.725866f, 0.763401f, 
		
		0.763401f, 0.725866f, 0.763401f, 
		-0.763401f, 0.725866f, -0.763401f, 
		0.763401f, 0.725866f, -0.763401f, 
		
		-1.01532f, 0.725866f, 0.0f, 
		0.0f, 0.725866f, 1.01532f, 
		1.01532f, 0.725866f, 0.0f, 
		
		0.0f, 0.725866f, -1.01532f 
	};

	const int faces[] = {
		3, 7, 15, 
		1, 6, 14, 
		0, 5, 13, 
		
		2, 4, 12, 
		7, 2, 10, 
		6, 3, 11, 
		
		5, 1, 9, 
		4, 0, 8, 
		11, 3, 15, 
		
		9, 1, 14, 
		8, 0, 13, 
		10, 2, 12, 
		
		15, 7, 10, 
		14, 6, 11, 
		13, 5, 9,

		12, 4, 8 
	};
	
	for(int i = 0; i < 48; ++i){
		_faces[i] = faces[i];
	}
	for(int i = 0; i < 48; ++i){
		_verts[i] = verts[i];
	}

	_indexCount = sizeof(_faces) / sizeof(_faces[0]);
	std::cout << "\nindex count: " << _indexCount << "\n\n";

	_stride = 3 * sizeof(float);
}

void Cylinder::initGL() {
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

GLuint Cylinder::getVAO() {
	if(!_doneInitGL)
		initGL();
	return _vao;
}

GLsizei Cylinder::getIndexCount() const {
	return _indexCount;
}
