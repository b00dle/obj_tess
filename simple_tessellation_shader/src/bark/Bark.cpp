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
	
	int i = 0;
	float pos = 0.0f;
	while(strip != _barkStrips.end()) {
		module = (*strip)->getBarkModules().begin();
		pos = 0.0f;
		while(module != (*strip)->getBarkModules().end()){
			if((*module)->type() == 1) {
				Fracture* fracture = (Fracture*) (*module);
				if(fracture->getExtension() > 1.0f){
					pos += 0.5f * fracture->getExtension();
					propagateDown(strip, fracture, pos);
					//propagateUp(strip, fracture, pos);
					pos += 0.5f * fracture->getExtension();
				}
				else
					pos += fracture->getExtension();
			}
			else
				pos += (*module)->getRestLength() + (*module)->getExtension();
			++module;
		}
		++strip;
		++i;
	}
}

void Bark::propagateDown(std::vector<BarkStrip*>::iterator const& strip, Fracture* fracture, float refPos){
	std::vector<BarkStrip*>::iterator next = strip;
	++next;
	if(strip == _barkStrips.end() || next == _barkStrips.end() || fracture->getExtension() <= 1.0f)
		return;
	else {
		std::list<BarkModule*>::iterator module = (*next)->getBarkModules().begin();
		float stripPos = 0.0f;
		while(module != (*next)->getBarkModules().end()){
			if((*module)->type() == 0 && refPos - stripPos < (*module)->getExtension() + (*module)->getRestLength()){
				Crust* crust = (Crust*) (*module);
				
				float crustLength1 = refPos - stripPos;
				float crustLength2 = crust->getExtension() + crust->getRestLength() - crustLength1;
				float fractureExtension = fracture->getExtension() - 1.0f;

				crustLength1 -= fractureExtension / 2.0f;
				crustLength2 -= fractureExtension / 2.0f;

				float resizeCoeff1 = crustLength1 / ( crust->getExtension() + crust->getRestLength() ); 
				float resizeCoeff2 = crustLength2 / ( crust->getExtension() + crust->getRestLength() );
				float threshold1 = crust->getThreshold() * resizeCoeff1;
				float threshold2 = crust->getThreshold() * resizeCoeff2;
				
				float crustStiffness1 = crust->getRestLength() * crust->getStiffness() / crustLength1; 
				float crustStiffness2 = crust->getRestLength() * crust->getStiffness() / crustLength2;

				(*next)->getBarkModules().insert(module, new Crust(crustLength1,
												0.0f,
												crustStiffness1,
												threshold1)
				);
			
				(*next)->getBarkModules().insert(module, new Fracture(0.0f,
														fractureExtension,
														fracture->getStiffness())
				);

				(*next)->getBarkModules().insert(module, new Crust(crustLength2,
												0.0f,
												crustStiffness2,
												threshold2)
				);

				module = (*next)->getBarkModules().erase(module);
				--module;
				--module;
				return propagateDown(next, (Fracture*) (*module), refPos); 
			}
			stripPos += (*module)->getExtension() + (*module)->getRestLength();
			++module;
		}
	}
}

void Bark::propagateUp(std::vector<BarkStrip*>::iterator const& strip, Fracture* fracture, float pos){

}
