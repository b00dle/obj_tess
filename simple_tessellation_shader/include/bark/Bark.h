#pragma once

#include "BarkStrip.h"
#include "Fracture.h"

#include <vector>

class Bark {
	public:
		Bark(void);
		Bark(float, float, std::vector<BarkStrip*> const&);
		Bark(Bark const&);
		~Bark(void);

		std::vector<BarkStrip*> const& getBarkStrips() const;

		void grow();
		void propagateFractures();

	private:
		void propagateDown(std::vector<BarkStrip*>::iterator const&, Fracture*, float);
		void propagateUp(std::vector<BarkStrip*>::iterator const&, Fracture*, float);

	private:
		float					_growthRate;
		float					_stiffness;
		std::vector<BarkStrip*> _barkStrips;

};

