#pragma once

#include "DSPixel.h"

class DSComparePixels {
	public:
		bool operator () (DSPixel const& p1, DSPixel const& p2) {
			return p1.distance > p2.distance;
		}
};
