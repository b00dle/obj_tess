#pragma once

#include "BarkStrip.h"

#include <vector>

class Bark {
	public:
		Bark(void);
		Bark(float, float, std::vector<BarkStrip*> const&);
		Bark(Bark const&);
		~Bark(void);

		void grow();
		void propagateFractures();

	private:
		float					_growthRate;
		float					_stiffness;
		std::vector<BarkStrip*> _barkStrips;

};

