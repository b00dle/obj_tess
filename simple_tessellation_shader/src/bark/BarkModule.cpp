#include "BarkModule.h"
#include <math.h>

#define EPSILON 0.00001f

BarkModule::BarkModule(void):
	_restLength(1.0f),
	_extension(0.0f),
	_stiffness(0.5f)
{
}

BarkModule::BarkModule(float restLength, float extension, float materialStiffness):
	_restLength(restLength),
	_extension(extension),
	_stiffness(0.0f)
{
	if(fabs(extension) > EPSILON)
		_stiffness = materialStiffness;
	else
		_stiffness = materialStiffness / restLength; 
}


BarkModule::~BarkModule(void) {
}

float	BarkModule::getRestLength() const {
	return _restLength;
}

float	BarkModule::getExtension()	const {
	return _extension;
}

float	BarkModule::getStiffness()	const {
	return _stiffness;
}

void BarkModule::extend(float e) {
	float oldExtension = _extension;
	_extension += e;
	_stiffness = _stiffness*oldExtension / _extension;
}