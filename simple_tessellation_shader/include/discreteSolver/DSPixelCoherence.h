#pragma once

#include "DSCoherentTexture.h"
#include "DSPixel.h"
#include <vector>

class DSCoherentTexture;
class DSPixelCoherence {
	public:
		DSPixelCoherence();
		DSPixelCoherence(DSCoherentTexture const&, nv::vec2i const&, std::vector<int> const&);
		DSPixelCoherence(DSCoherentTexture const&, DSPixelCoherence const&, nv::vec2i const&, std::vector<int> const&);

		std::vector<std::vector<nv::vec2i*>> const& getNeighbourPos() const;

	private:
		std::vector<int>						_neighbourSizes;
		std::vector<std::vector<nv::vec2i*>>	_neighbourPos;
		int										_neighbourPosDim;
		nv::vec2i								_pixelPos;
};
