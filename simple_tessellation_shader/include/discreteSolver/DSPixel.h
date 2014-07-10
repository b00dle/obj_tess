#pragma once

#include "sdk_swak.h"

struct DSPixel {
	nv::vec2i position;
	int distance;

	DSPixel():
		position(0,0),
		distance(0)
	{}

	DSPixel(nv::vec2i pos, int dist):
		position(pos),
		distance(dist)
	{}
};