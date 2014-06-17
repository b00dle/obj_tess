#pragma once

#include "BarkStrip.h"

#include <vector>

class Bark {
	public:
		Bark(void);
		Bark(float, float, std::vector<BarkStrip*> const&);
		Bark(Bark const&);
		~Bark(void);

	private:
		float					_growthRate;
		float					_stiffness;
		std::vector<BarkStrip*> _barkStrips;

};

