#pragma once

#include "BarkModule.h"

#include <list>

class BarkStrip {
	public:
		BarkStrip(void);
		BarkStrip(std::list<BarkModule*> const&);
		BarkStrip(BarkStrip const&);
		~BarkStrip(void);

		std::list<BarkModule*>& getBarkModules();

		void extendLength(float, float);

	private:
		std::list<BarkModule*> _barkModules;
};

