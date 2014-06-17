#include "Crust.h"

Crust::Crust(void):
	BarkModule(),
	_threshold(0.0f)
{
}

Crust::Crust(float restLength, float extension, float stiffness, float threshold):
	BarkModule(restLength, extension, stiffness),
	_threshold(threshold)
{
}

Crust::Crust(Crust const& crust):
	BarkModule(crust._restLength, crust._extension, crust._stiffness),
	_threshold(crust._threshold)
{
}

Crust::~Crust(void) {
	BarkModule::~BarkModule();
}

/*virtual*/ unsigned Crust::type() const {
	return 0;
}

/*virtual*/ bool Crust::solveStress() const {
	return (_restLength + _extension) / _restLength > _threshold;
}

float Crust::getThreshold() const {
	return _threshold;
}