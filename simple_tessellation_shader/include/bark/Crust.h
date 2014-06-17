#pragma once

#include "barkmodule.h"

class Crust : public BarkModule {
	public:
		Crust(void);
		Crust(float, float, float, float);
		Crust(Crust const&);
		~Crust(void);

		/*virtual*/ unsigned type() const;
		/*virtual*/ bool solveStress() const;
		
		float getThreshold() const;

	private:
		float _threshold;
};

