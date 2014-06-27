#include "Scene3D.h"

#include <iostream>
#include <fstream>
#include <algorithm>

Scene3D::Scene3D(const char* filePath):
	_filePath(filePath),
	_vao(),
	_doneInitGL(false),
	_meshCount(0),
	_min(),
	_max(),
	_VBOpositions(),
	_VBOnormals(),
	_VBOtexCoords(),
	_VBOindices(),
	_indexCount()
{
	if(!init())
		std::cout << "ERROR loading file: " << filePath << "\n";
	else
		std::cout << "LOADED file: " << filePath << "\n";
}

Scene3D::~Scene3D()
{}

GLuint Scene3D::getVAO(int i) {
	if(!_doneInitGL)
		initGL();
	return _vao[i];
}

GLsizei Scene3D::getIndexCount(int i) const {
	return _indexCount[i] * 3;
}

bool Scene3D::init() {
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

bool Scene3D::initGL() {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile( _filePath, aiProcessPreset_TargetRealtime_Quality);

	if(!scene)	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}
	
	if(!scene->mNumMeshes > 0)
		return false;

	_meshCount = scene->mNumMeshes;

	//resize property vectors
	_vao.resize(_meshCount);
	_min.resize(_meshCount);
	_max.resize(_meshCount);
	_VBOpositions.resize(_meshCount);
	_VBOnormals.resize(_meshCount);
	_VBOtexCoords.resize(_meshCount);
	_VBOindices.resize(_meshCount);
	_indexCount.resize(_meshCount);

	for(int i = 0; i < scene->mNumMeshes; ++i){
	
		const aiMesh* mesh = scene->mMeshes[i];
		const aiVector3D* verts = mesh->mVertices;

		_min[i] = aiVector3D(1000.0,1000.0,1000.0);
		_min[i] = aiVector3D(-1000.0,-1000.0,-1000.0);
		
		for(int a = 0; a < mesh->mNumVertices; ++a){
			_min[i].x = std::min(verts[a].x,_min[i].x);
			_min[i].y = std::min(verts[a].y,_min[i].y);
			_min[i].z = std::min(verts[a].z,_min[i].z);

			_max[i].x = std::max(verts[a].x,_max[i].x);
			_max[i].y = std::max(verts[a].y,_max[i].y);
			_max[i].z = std::max(verts[a].z,_max[i].z);
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
		
		_indexCount[i] = scene->mMeshes[0]->mNumFaces;

		//VAO
		glGenVertexArrays(1, &_vao[i]);
		glBindVertexArray(_vao[i]);

		//VBO indices
		glGenBuffers(1, &_VBOindices[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _VBOindices[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->mNumFaces * 3, faceArray, GL_STATIC_DRAW);

		//VBO positions
		if (mesh->HasPositions()) {
			glGenBuffers(1, &_VBOpositions[i]);
			glBindBuffer(GL_ARRAY_BUFFER, _VBOpositions[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*mesh->mNumVertices, mesh->mVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}

		//VBO normals
		if (mesh->HasNormals()) {
			glGenBuffers(1, &_VBOnormals[i]);
			glBindBuffer(GL_ARRAY_BUFFER, _VBOnormals[i]);
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
			glGenBuffers(1, &_VBOtexCoords[i]);
			glBindBuffer(GL_ARRAY_BUFFER, _VBOtexCoords[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*mesh->mNumVertices, texCoords, GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
		}

		// unbind buffers
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	_doneInitGL = true;

	return true;
}