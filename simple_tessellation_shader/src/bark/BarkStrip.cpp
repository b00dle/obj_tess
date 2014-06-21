#include "BarkStrip.h"
#include "Crust.h"
#include "Fracture.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

BarkStrip::BarkStrip(void):
	_barkModules(),
	_length(0.0f)
{
}

BarkStrip::BarkStrip(std::list<BarkModule*> const& barkModules):
	_barkModules(),
	_length(0.0f)
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

	for(auto module : _barkModules) {
		_length += module->getExtension() + module->getRestLength();
	}
	
}

BarkStrip::BarkStrip(BarkStrip const& barkStrip):
	_barkModules(),
	_length(barkStrip._length)
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

float BarkStrip::getLength() const {
	return _length;
}

void BarkStrip::extendLength(float e, float globalStiffness) {
	float ePartial = e / _barkModules.size(); //equally subdivide e
	_length += e;

	std::list<BarkModule*>::iterator module = _barkModules.begin();
	while(module != _barkModules.end()) {
	
		//float eModule = ePartial * (*(module))->getStiffness() / globalStiffness; //scale e dependent on module stiffness
		(*module)->extend(ePartial);
		
		if((*module)->solveStress()) {
			
			Crust* crust = (Crust*) (*module);
			float kf = 2000.0f/crust->getRestLength();
			float ks = crust->getStiffness();
			float eRemainder = crust->getExtension() * (kf) / (ks + kf);
			
			srand(time(0)*10000000);
			float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			//float cutPos = (0.5f + 1.0f*r);

			float fractureExtension = crust->getExtension() - eRemainder;
			float crustLength1    = crust->getRestLength() * r;
			float crustLength2    = crust->getRestLength() - crustLength1;
			float crustStiffness1 = crust->getRestLength() * crust->getStiffness() / crustLength1;
			float crustStiffness2 = crust->getRestLength() * crust->getStiffness() / crustLength2;
			float crustThreshold = crust->getThreshold();
			
			/*
			srand(time(NULL));		
			
			float cThreshold;
			if((float) (rand() % 2) > 0.5f)
				cThreshold = crustThreshold / 2.0f + (1.0f / (float) (rand() % 4 + 1) );
			else
				cThreshold = crustThreshold / 2.0f - (1.0f / (float) (rand() % 4 + 1) );
			*/
			
			_barkModules.insert(module, new Crust(crustLength1 + eRemainder * r,
												0.0f,
												crustStiffness1,
												crustThreshold * r)
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

			_barkModules.insert(module, new Crust(crustLength2 + eRemainder * (1 - r),
												0.0f,
												crustStiffness2,
												crustThreshold * (1 - r))
			);

			module = _barkModules.erase(module);
		}
		
		else
			++module;
	
	}
}
