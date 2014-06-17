#include "Fracture.h"

Fracture::Fracture(void) :
	BarkModule()
{
}

Fracture::Fracture(float restLength, float extension, float stiffness) :
	BarkModule(restLength, extension, stiffness)
{
}

Fracture::Fracture(Fracture const& fracture):
	BarkModule(fracture._restLength, fracture._extension, fracture._stiffness)
{
}

Fracture::~Fracture(void){
	BarkModule::~BarkModule();
}

/*virtual*/ unsigned Fracture::type() const {
	return 1;
}

/*virtual*/ bool Fracture::solveStress() const {
	return false;
}
