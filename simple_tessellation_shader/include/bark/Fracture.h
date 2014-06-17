#pragma once

#include "barkmodule.h"

class Fracture : public BarkModule {
	public:
		Fracture(void);
		Fracture(float, float, float);
		Fracture(Fracture const&);
		~Fracture(void);

		/*virtual*/ unsigned type() const;
		/*virtual*/ bool solveStress() const;
};

