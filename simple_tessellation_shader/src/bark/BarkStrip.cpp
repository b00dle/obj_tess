#include "BarkStrip.h"
#include "Crust.h"
#include "Fracture.h"
#include <stdlib.h>
#include <time.h>

BarkStrip::BarkStrip(void):
	_barkModules()
{
}

BarkStrip::BarkStrip(std::list<BarkModule*> const& barkModules):
	_barkModules()
{
	Crust* crust = nullptr;
	Fracture* fracture = nullptr;
	for(auto module : barkModules) {
		if(module->type() == 0) {
			crust = (Crust*) module;
			_barkModules.push_back( new Crust(*crust) );
		}
		if(module->type() == 1) {
			fracture = (Fracture*) module;
			_barkModules.push_back( new Fracture(*fracture) );
		}
	}
	crust = nullptr;
	fracture = nullptr;
	delete crust;
	delete fracture;
}

BarkStrip::BarkStrip(BarkStrip const& barkStrip):
	_barkModules()
{
	Crust* crust = nullptr;
	Fracture* fracture = nullptr;
	for(auto module : barkStrip._barkModules) {
		if(module->type() == 0) {
			crust = (Crust*) module;
			_barkModules.push_back( new Crust(*crust) );
		}
		if(module->type() == 1) {
			fracture = (Fracture*) module;
			_barkModules.push_back( new Fracture(*fracture) );
		}
	}
	crust = nullptr;
	fracture = nullptr;
	delete crust;
	delete fracture;
}

BarkStrip::~BarkStrip(void) {
}

std::list<BarkModule*>& BarkStrip::getBarkModules(){
	return _barkModules;
}

void BarkStrip::extendLength(float e, float globalStiffness) {
	float ePartial = e / _barkModules.size(); //equally subdivide e
	std::list<BarkModule*>::iterator module = _barkModules.begin();
	
	while(module != _barkModules.end()) {
	
		//float eModule = ePartial * (*(module))->getStiffness() / globalStiffness; //scale e dependent on module stiffness
		(*module)->extend(ePartial);
		
		if((*module)->solveStress()) {
			
			Crust* crust = (Crust*) (*module);
			float kf = 10.0f/crust->getRestLength();
			float ks = crust->getStiffness();
			float eRemainder = crust->getExtension() * (kf) / (ks + kf);
			
			float fractureExtension = crust->getExtension() - eRemainder;
			float crustLength    = crust->getRestLength() / 2.0f;
			float crustStiffness = crust->getRestLength() * crust->getStiffness() / crustLength;
			float crustThreshold = crust->getThreshold();
			
			/*
			srand(time(NULL));		
			
			float cThreshold;
			if((float) (rand() % 2) > 0.5f)
				cThreshold = crustThreshold / 2.0f + (1.0f / (float) (rand() % 4 + 1) );
			else
				cThreshold = crustThreshold / 2.0f - (1.0f / (float) (rand() % 4 + 1) );
			*/
			
			_barkModules.insert(module, new Crust(crustLength + eRemainder/2.0f,
												0.0f,
												crustStiffness,
												crustThreshold / 2.0f)
			);
			
			_barkModules.insert(module, new Fracture(0.0f,
													fractureExtension,
													kf)
			);

			/*
			if((float) (rand() % 2) > 0.5f)
				cThreshold = crustThreshold / 2.0f * (1.0f / (float) (rand() % 4 + 1) );
			else
				cThreshold = crustThreshold / 2.0f * -(1.0f / (float) (rand() % 4 + 1) );
			*/

			_barkModules.insert(module, new Crust(crustLength + eRemainder/2.0f,
												0.0f,
												crustStiffness,
												crustThreshold / 2.0f)
			);

			module = _barkModules.erase(module);
		}
		
		else
			++module;
	
	}
}
