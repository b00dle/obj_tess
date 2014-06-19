#include "Bark.h"
#include "Fracture.h"
#include "Crust.h"

Bark::Bark(void):
	_growthRate(0.0f),
	_stiffness(0.0f),
	_barkStrips()
{
}

Bark::Bark(float growthRate, float stiffness, std::vector<BarkStrip*> const& barkStrips):
	_growthRate(growthRate),
	_stiffness(stiffness),
	_barkStrips()
{
	for(auto strip : barkStrips)
		_barkStrips.push_back(new BarkStrip( *strip ));
}

Bark::Bark(Bark const& bark):
	_growthRate(bark._growthRate),
	_stiffness(bark._stiffness),
	_barkStrips()
{
	for(auto strip : bark._barkStrips)
		_barkStrips.push_back(new BarkStrip( *strip ));
}


Bark::~Bark(void) {
}

void Bark::grow() {
	for(auto strip : _barkStrips)
		strip->extendLength(_growthRate, _stiffness);
	
	propagateFractures();
}

void Bark::propagateFractures() {
	std::vector<BarkStrip*>::iterator strip = _barkStrips.begin();
	std::list<BarkModule*>::iterator module = (*strip)->getBarkModules().begin();
	float pos = 0.0f;
	while(strip != _barkStrips.end()) {
		if((*module)->type() == 1) {
			Fracture* fracture = (Fracture*) (*module);
		}
	}
}
