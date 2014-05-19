#include "Object.hpp"
#include <iostream>
#include <fstream>
#include <sstream>      
#include <string>
#include <vector>

Object::Object(const char* path):
	_vertices(),
	_normals(),
	_elements(),
	_vao(),
	_VBOvertices(),
	_VBOnormals(),
	_VBOindices(),
	_indexCount(0),
	_doneInitGL(false)
{
	if(!loadOBJ(path))
		std::cout << "object failed to load! check for misspelled filepath or malformed file.\n";
	else
		printInfo(path);

}

Object::~Object()
{}

void Object::printInfo(const char* path) const{
	std::cout << "\n____________MESH INFO_____________\n";
	std::cout << "loaded obj from path: " << path << std::endl;
	std::cout << "number of vertices: " << _vertices.size() << std::endl;
	std::cout << "number of normals: " << _normals.size() << std::endl;
	std::cout << "number of indices: " << _elements.size() << std::endl;
	std::cout << "__________________________________\n";
}

void Object::init() {
	//TODO!?
}

void Object::initGL() {
//initializes openGL relevant stuff (VAO, VBOs, buffer sizes...)
	//create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	int* faces = new int[_elements.size()];
	float* verts = new float[_vertices.size()];

	std::ofstream objFile;
	objFile.open("obj_file_formatted.txt");

	for(int i = 0; i < _elements.size(); ++i){
		if(i % 3 == 0 && i != 0)
			objFile << "\n";
		objFile << _elements[i] << ", ";
		faces[i] = _elements[i];
	}
	for(int i = 0; i < _vertices.size(); ++i){
		if(i % 3 == 0 && i != 0)
			objFile << "\n";
		objFile << _vertices[i] << "f, ";
		verts[i] = _vertices[i];
	}

	objFile.close();


	//std::cout << "faces count (array): " << sizeof(faces) / sizeof(faces[0]) << std::endl;

	//create VBO for positions
	glGenBuffers(1, &_VBOvertices);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOvertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), verts, GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	//create VBO for normals
	/*glGenBuffers(1, &_VBOnormals);
	glBindBuffer(GL_ARRAY_BUFFER, _VBOnormals);
	glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(nv::vec3<float>), &_normals[0], GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);*/
	
	//create VBO for indices
	glGenBuffers(1, &_VBOindices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBOindices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_elements), faces, GL_STATIC_DRAW);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	//_indexCount = size/sizeof(GLuint);
	_indexCount = _elements.size();
	std::cout << "\nindex count: " << _indexCount << "\n\n";

	glBindVertexArray(0); //unbind vertex array
	
	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

	_doneInitGL = true;
}

GLuint Object::getVAO() {
	if(!_doneInitGL)
		initGL();
	return _vao;
}

GLsizei Object::getIndexCount() const {
	return _indexCount;
}

bool Object::loadOBJ(const char* filename) { 
//fills _vertices, _elements & _normals members from given file in obj format
	std::ifstream in(filename, std::ios::in);
	if (!in) { std::cerr << "Cannot open " << filename << std::endl; return false; }
 
	std::string line;
	while (std::getline(in, line)) {
		if (line.substr(0,2) == "v ") {
			std::istringstream s(line.substr(2));

			_vertices.push_back(float());
			s >> _vertices.back();
			
			_vertices.push_back(float());
			s >> _vertices.back();
			
			_vertices.push_back(float());
			s >> _vertices.back();
			
		}  else if (line.substr(0,2) == "f ") {
			std::istringstream s(line.substr(2));
			std::string fixedLine = "";
			for(int i = 0; i < s.str().size(); ++i){ //splits '//' from the read line 
				if(s.str()[i] == '/'){
					fixedLine += " ";
					++i;
				}
				else
					fixedLine += s.str()[i];
			}
			std::istringstream fixedS(fixedLine);
			GLushort a,b,c,dummy;
			fixedS >> a;
			fixedS >> dummy; //index of face (not needed in IBO)
			fixedS >> b;
			fixedS >> dummy; //index of face (not needed in IBO)
			fixedS >> c;
			a--; b--; c--;
			_elements.push_back(a); _elements.push_back(b); _elements.push_back(c);
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}
 
	//_normals.resize(_vertices.size(), nv::vec3<float>(0.0, 0.0, 0.0));
	for (unsigned i = 0; i < _elements.size(); i+=3) { 
	//at the moment normals are not being read, but calculated
		/*GLushort ia = _elements[i];
		GLushort ib = _elements[i+1];
		GLushort ic = _elements[i+2];
		nv::vec3<float> lhs = nv::vec3<float>(_vertices[ib]) - nv::vec3<float>(_vertices[ia]);
		nv::vec3<float> rhs = nv::vec3<float>(_vertices[ic]) - nv::vec3<float>(_vertices[ia]);
		nv::vec3<float> normal = nv::normalize(my_cross(lhs,rhs));
		_normals[ia] = _normals[ib] = _normals[ic] = normal;*/
	}

return true;
}

nv::vec3<float> Object::my_cross(nv::vec3<float> const& lhs, nv::vec3<float> const& rhs) const { 
//implementation of cross product since cross somehow runs into bugs
	nv::vec3<float> res(0.0f,0.0f,0.0f);
	res[0] = lhs[1]*rhs[2]-lhs[2]*rhs[1];
	res[1] = lhs[2]*rhs[0]-lhs[0]*rhs[2];
	res[2] = lhs[0]*rhs[1]-lhs[1]*rhs[0];
	return res;
}