#pragma once

#include "BarkModule.h"

#include <list>

class BarkStrip {
	public:
		BarkStrip(void);
		BarkStrip(std::list<BarkModule*> const&);
		BarkStrip(BarkStrip const&);
		~BarkStrip(void);

		void extendLength(float, float);

	private:
		std::list<BarkModule*> _barkModules;
};

