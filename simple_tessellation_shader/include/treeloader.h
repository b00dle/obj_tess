#pragma once
#include<vector>
#include "sdk_swak.h"
#include "contour.h"
#include "GeneralizedCylinder.h"

class TreeLoader
{
public:
	TreeLoader();
	~TreeLoader();

	void readTreeDescr();
	GeneralizedCylinder getTreeDescr();

private:

	std::vector<nv::matrix4f>	_paths;
	Contour						_contour;
	std::vector<float>			_thickness;
	std::vector<float>			_water;
	std::vector<float>			_ages;
};

