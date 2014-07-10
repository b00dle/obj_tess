#pragma once

#include "DSTexture.h"
#include "DSPixel.h"
#include "DSComparePixel.h"
#include "DSIndexedTexture.h"
#include "DSPixelCoherence.h"
#include <queue>

class DSIndexedTexture;
class DSCoherentTexture : public DSTexture {
	public:
		DSCoherentTexture();
		DSCoherentTexture(nv::vec2i const&);
		DSCoherentTexture(Image*);
		DSCoherentTexture(nv::vec2i const&, nv::vec2i const&, DSTexture const&);
		DSCoherentTexture(Image*, DSCoherentTexture const&);

		int							getNeighbourSize(int) const;
		std::vector<int> const&		getNeighbourSizes() const;

		std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> findClosestNeighbourhood(DSIndexedTexture const&, nv::vec2i const&, int, int);

		void findCandidates(DSIndexedTexture const&, nv::vec2i const&, int, int, std::vector<nv::vec2i*>&);
		void createCoherence();
		
	private:
		std::vector<std::vector<DSPixelCoherence*>>		_pixelCoherences;
		std::vector<int>								_neighbourSizes;
};