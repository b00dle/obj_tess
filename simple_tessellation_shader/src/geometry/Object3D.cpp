#include "Object3D.h"

#include <iostream>
#include <fstream>
#include <algorithm>

Object3D::Object3D(const char* filePath):
	_filePath(filePath),
	_vao(),
	_doneInitGL(false),
	_min(1000.0,1000.0,1000.0),
	_max(-1000.0,-1000.0,-1000.0),
	_VBOpositions(),
	_VBOnormals(),
	_VBOtexCoords(),
	_VBOindices(),
	_indexCount(0)
{
	if(!init())
		std::cout << "ERROR loading file: " << filePath << "\n";
	else
		std::cout << "LOADED file: " << filePath << "\n";
}

Object3D::~Object3D()
{}

GLuint Object3D::getVAO() {
	if(!_doneInitGL)
		initGL();
	return _vao;
}

GLsizei Object3D::getIndexCount() const {
	return _indexCount * 3;
}

bool Object3D::init() {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _filePath, aiProcessPreset_TargetRealtime_Quality);

	std::ifstream fin(_filePath);
	if(!fin.fail()) {
		fin.close();
	}
	else{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	return true;
}

bool Object3D::initGL() {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _filePath, aiProcessPreset_TargetRealtime_Quality);

	if(!scene)	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}
	
	if(!scene->mNumMeshes > 0)
		return false;
	
	const aiMesh* mesh = scene->mMeshes[0];
	const aiVector3D* verts = mesh->mVertices;

	for(int i = 0; i < mesh->mNumVertices; ++i){
		_min.x = std::min(verts[i].x,_min.x);
		_min.y = std::min(verts[i].y,_min.y);
		_min.z = std::min(verts[i].z,_min.z);

		_max.x = std::max(verts[i].x,_max.x);
		_max.y = std::max(verts[i].y,_max.y);
		_max.z = std::max(verts[i].z,_max.z);
	}
	
	/*
	aiVector3D right(_max.x, _min.y, _min.z);
	aiVector3D up(_min.x, _max.y, _min.z);
	aiVector3D left(_min.x, _min.y, _max.z);

	aiVector3D vRight	= right - _min;
	aiVector3D vUp		= up - _min;
	aiVector3D vLeft	= left - _min;

	float volume = vRight.Length() * vUp.Length() * vLeft.Length();	
	*/

	unsigned int* faceArray;
	faceArray = (unsigned int*) malloc(sizeof(unsigned int) * mesh->mNumFaces * 3);
	unsigned int faceIndex = 0;
		
	for(unsigned int t = 0; t < mesh->mNumFaces; ++t) {
		const aiFace* face = &mesh->mFaces[t];
		memcpy(&faceArray[faceIndex], face->mIndices, 3 * sizeof(unsigned int));
		faceIndex += 3;
	}
		
	_indexCount = scene->mMeshes[0]->mNumFaces;

	//VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//VBO indices
	glGenBuffers(1, &_VBOindices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBOindices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

	//VBO positions
	if (mesh->HasPositions()) {
		glGenBuffers(1, &_VBOpositions);
		glBindBuffer(GL_ARRAY_BUFFER, _VBOpositions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//VBO normals
	if (mesh->HasNormals()) {
		glGenBuffers(1, &_VBOnormals);
		glBindBuffer(GL_ARRAY_BUFFER, _VBOnormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mNormals, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//VBO texcoords
	if (mesh->HasTextureCoords(0)) {
		float *texCoords = (float *)malloc(sizeof(float)*2*mesh->mNumVertices);
		for (unsigned int k = 0; k < mesh->mNumVertices; ++k) {

			texCoords[k*2]   = mesh->mTextureCoords[0][k].x;
			texCoords[k*2+1] = mesh->mTextureCoords[0][k].y; 
				
		}
		glGenBuffers(1, &_VBOtexCoords);
		glBindBuffer(GL_ARRAY_BUFFER, _VBOtexCoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	
	_doneInitGL = true;

	return true;
}