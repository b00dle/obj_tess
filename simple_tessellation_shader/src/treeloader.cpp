#include "treeLoader.h"
#include "Path.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

TreeLoader::TreeLoader()
{
}

TreeLoader::~TreeLoader()
{
}

void TreeLoader::readTreeDescr(){

	std::ifstream in("genCylDesc.txt", std::ios::in);
	if (!in) { std::cerr << "Cannot open file! "  << std::endl;}

	std::string line;
	std::getline(in, line);

	unsigned int i = 0;
	if ( line.substr(0,14) == "Cylindercount "){
		std::stringstream s(line.substr(14));
		s >> i;
	}

	while (std::getline(in, line)) 
	{
		//std::cout << line << std::endl;
		//////////////Radii////////////////
		if (line.substr(0,2) == "r ")
		{
			std::istringstream s(line.substr(2));
			_thickness.push_back(float());
			s >> _thickness.back();

		}  
		if (line.substr(0,2) == "p ") 
		{
			std::istringstream s(line.substr(2));
			_paths.push_back(nv::matrix4f());
			s >> _paths.back()._11; 
			s >> _paths.back()._12;
			s >> _paths.back()._13;
			s >> _paths.back()._14;
			s >> _paths.back()._21;
			s >> _paths.back()._22;
			s >> _paths.back()._23;
			s >> _paths.back()._24;
			s >> _paths.back()._31;
			s >> _paths.back()._32;
			s >> _paths.back()._33;
			s >> _paths.back()._34;
			s >> _paths.back()._41;
			s >> _paths.back()._42;
			s >> _paths.back()._43;
			s >> _paths.back()._44;

		}
	}


}

GeneralizedCylinder TreeLoader::getTreeDescr(){

	//PATHS//////////////
	Path _path;
	for (int i = 0; i < _paths.size(); i++){
		nv::matrix4f temp;
		temp = _paths[i];
		_path.addSegment(temp);
	}
	_path.calculate();

	////CONTOUR/////
	Contour contour;

	////THICKNESS///////
	std::vector<float> thickness;
	for (int j = 0; j < _thickness.size(); j++){
		float temp = _thickness[j];
		thickness.push_back(temp);
	}

	////Water/////
	std::vector<float> water;
	for (int k = 0; k < _thickness.size(); k++)
		water.push_back(1.0);

	////Ages/////
	std::vector<float> ages;
	for (int l = 0; l < _thickness.size(); l++){
		float temp = _thickness[l] / 8;
		ages.push_back(temp);
	}

	GeneralizedCylinder genCyl(_path, contour, thickness, water,ages);



	return genCyl;
}