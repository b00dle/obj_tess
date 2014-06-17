#pragma once

#include "Bark.h"

class BarkTexGenerator {
	public:
		BarkTexGenerator(void);
		BarkTexGenerator(Bark* bark);
		~BarkTexGenerator(void);
	private:
		Bark* _bark;
};

