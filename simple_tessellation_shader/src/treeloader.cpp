#include "treeLoader.h"
#include "Path.h"
#include <iostream>
#include <fstream>
#include <string>
TreeLoader::TreeLoader(const char* myfile)
{
	readTreeDescr(myfile);
}


TreeLoader::~TreeLoader()
{
}

bool readTreeDescr(const char* myfile){
	
	std::string line;
	std::ifstream myFile (myfile);
	if (myFile.is_open())
	{
		while (getline (myFile,line) )
		{
			std::cout << line << '\n';
		}
		myFile.close();
	}

	else std::cout << "Unable to open file";

}

GeneralizedCylinder TreeLoader::getTreeDescr()const{

	std::vector<GeneralizedCylinder> genCyls;
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
	for (int k = 0; k < _thickness.size(); k++){
		float temp = _thickness[k] / 10;
		ages.push_back(temp);
	}

	GeneralizedCylinder genCyl(_path, contour, thickness, water,ages);

}