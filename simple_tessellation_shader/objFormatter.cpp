#include "objFormatter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>      
#include <string>
#include <vector>

ObjFormatter::ObjFormatter(const char* path):
	_vertices(),
	_normals(),
	_texCoords(),
	_elements(),
	_numVertices(),
	_numNormals(),
	_numTexCoords(),
	_numElements()
{
	if(!readOBJ(path))
		std::cout << "ObjFormatter failed to load! check for misspelled filepath or malformed file.\n";
	else {
		printInfo(path);
		writeOBJ(path);
	}

}

ObjFormatter::~ObjFormatter()
{}

void ObjFormatter::printInfo(const char* path) const{
	std::cout << "\n____________MESH INFO_____________\n";
	std::cout << "loaded obj from path: " << path << std::endl;
	std::cout << "number of vertices: " << _numVertices << std::endl;
	std::cout << "number of normals: " << _numNormals << std::endl;
	std::cout << "number of indices: " << _numElements << std::endl;
	std::cout << "number of texCoords: " << _numTexCoords << std::endl;
	std::cout << "__________________________________\n";
}

bool ObjFormatter::readOBJ(const char* filename) { 
//fills _vertices, _elements & _normals members from given file in obj format

	bool hasTexCoords = false;
	std::vector<int> tcIndices;
	std::vector<int> nIndices;
	std::vector<nv::vec2f> tempTexCoords;
	std::vector<nv::vec3f> tempNormals;

	std::ifstream in(filename, std::ios::in);
	if (!in) { std::cerr << "Cannot open " << filename << std::endl; return false; }
 
	std::string line;
	while (std::getline(in, line)) {

		//////////////VERTS////////////////
		if (line.substr(0,2) == "v ") {
			std::istringstream s(line.substr(2));

			_vertices.push_back(float());
			s >> _vertices.back();
			
			_vertices.push_back(float());
			s >> _vertices.back();
			
			_vertices.push_back(float());
			s >> _vertices.back();
			
		}  

		//////////////FACES///////////////////
		else if (line.substr(0,2) == "f ") {
			std::istringstream s(line.substr(2));
			std::string fixedLine = "";
			bool slashOpen = false;
			for(int i = 0; i < s.str().size(); ++i){ //splits '//' from the read line 
				if(s.str()[i] == '/'){
					if(!hasTexCoords && s.str()[i+1] == '/')
						hasTexCoords = true;
					else if(!hasTexCoords)
						++i;
					fixedLine += " ";
				}
				else
					fixedLine += s.str()[i];
			}

			std::istringstream fixedS(fixedLine);
			
			if(hasTexCoords){
				int va,vb,vc,ta,tb,tc,na,nb,nc;
				fixedS >> va;
				fixedS >> ta;
				fixedS >> na; //index of face (not needed in IBO)
				fixedS >> vb;
				fixedS >> tb;
				fixedS >> nb; //index of face (not needed in IBO)
				fixedS >> vc;
				fixedS >> tc;
				fixedS >> nc;

				va--; vb--; vc--;
				_elements.push_back(va); _elements.push_back(vb); _elements.push_back(vc);

				ta--; tb--; tc--;
				tcIndices.push_back(ta); tcIndices.push_back(tb); tcIndices.push_back(tc);

				na--; nb--; nc--;
				nIndices.push_back(na); nIndices.push_back(nb); nIndices.push_back(nc);
			}
			else {
				int va,vb,vc,na,nb,nc;
				fixedS >> va;
				fixedS >> na; //index of face (not needed in IBO)
				fixedS >> vb;
				fixedS >> nb; //index of face (not needed in IBO)
				fixedS >> vc;
				fixedS >> nc;
				va--; vb--; vc--;
				_elements.push_back(va); _elements.push_back(vb); _elements.push_back(vc);

				na--; nb--; nc--;
				_elements.push_back(na); _elements.push_back(nb); _elements.push_back(nc);
			
			}
		}

		//////////NORMALS////////////////
		else if (line.substr(0,2) == "vn") {
			std::istringstream s(line.substr(2));

			tempNormals.push_back(nv::vec3f());
			s >> tempNormals.back().x;
			s >> tempNormals.back().y;
			s >> tempNormals.back().z;
		}

		//////////TEXCOORDS////////////////
		else if (line.substr(0,2) == "vt") {
			if(!hasTexCoords)
				hasTexCoords = !hasTexCoords;

			std::istringstream s(line.substr(2));

			tempTexCoords.push_back(nv::vec2f());
			s >> tempTexCoords.back().x;
			s >> tempTexCoords.back().y;
		}

		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}
	
	/////////////////TEXCOORDS & NORMALS/////////////////
	if(hasTexCoords){
		_texCoords.resize(tcIndices.size() * 2, 0.0f);
		int j = 0;
		for(int i = 0; i < tcIndices.size(); ++i){
			_texCoords[j] = tempTexCoords[tcIndices[i]].x;
			++j;
			_texCoords[j] = tempTexCoords[tcIndices[i]].y;
			++j;
		}
	}
	_normals.resize(nIndices.size() * 3, 0.0f);
	int j = 0;
	for(int i = 0; i < nIndices.size(); ++i){
		_normals[j] = tempNormals[nIndices[i]].x;
		++j;
		_normals[j] = tempNormals[nIndices[i]].y;
		++j;
		_normals[j] = tempNormals[nIndices[i]].z;
		++j;
	}

	/////////////////NUMS/////////////////
	_numVertices  = _vertices.size();
	_numElements  = _elements.size();
	_numNormals   = _normals.size();
	_numTexCoords = _texCoords.size();
	
	return true;
}

bool ObjFormatter::writeOBJ(const char* path){
	
	std::ofstream objFile;
	objFile.open("obj_file_formatted.txt");

	objFile << ">>>MESH INFO<<<\n";
	objFile << "loaded obj from path: " << path << std::endl;
	objFile << "number of vertices: " << _numVertices << std::endl;
	objFile << "number of normals: " << _numNormals << std::endl;
	objFile << "number of indices: " << _numElements << std::endl;
	objFile << "number of texCoords: " << _numTexCoords << std::endl;

	objFile << "\n\n>>>VERTICES<<<\n";
	for(int i = 0; i < _vertices.size(); ++i){
		if(i % 3 == 0 && i != 0)
			objFile << "\n";
		if(i < _vertices.size() - 1)
			objFile << _vertices[i] << "f, ";
		else
			objFile << _vertices[i] << "f";
	}

	objFile << "\n\n>>>FACES<<<\n";
	for(int i = 0; i < _elements.size(); ++i){
		if(i % 3 == 0 && i != 0)
			objFile << "\n";
		if(i < _elements.size() - 1)
			objFile << _elements[i] << ", ";
		else
			objFile << _elements[i];
	}

	objFile << "\n\n>>>NORMALS<<<\n";
	for(int i = 0; i < _normals.size(); ++i){
		if(i % 3 == 0 && i != 0)
			objFile << "\n";
		if(i < _normals.size() - 1)
			objFile << _normals[i] << "f, ";
		else
			objFile << _normals[i] << "f";
	}

	objFile << "\n\n>>>TEXCOORDS<<<\n";
	for(int i = 0; i < _texCoords.size(); ++i){
		if(i % 2 == 0 && i != 0)
			objFile << "\n";
		if(i < _texCoords.size() - 1)
			objFile << _texCoords[i] << "f, ";
		else
			objFile << _texCoords[i] << "f";
	}

	objFile.close();

	return true;
}

nv::vec3<float> ObjFormatter::my_cross(nv::vec3<float> const& lhs, nv::vec3<float> const& rhs) const { 
//implementation of cross product since cross somehow runs into bugs
	nv::vec3<float> res(0.0f,0.0f,0.0f);
	res[0] = lhs[1]*rhs[2]-lhs[2]*rhs[1];
	res[1] = lhs[2]*rhs[0]-lhs[0]*rhs[2];
	res[2] = lhs[0]*rhs[1]-lhs[1]*rhs[0];
	return res;
}