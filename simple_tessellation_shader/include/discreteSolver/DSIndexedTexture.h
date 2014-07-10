#pragma once

#include "DSTexture.h"
#include "DSCoherentTexture.h"
#include "Image.h"

#include "sdk_swak.h"
#include <vector>

class DSCoherentTexture;
class DSPixelCoherence;
class DSIndexedTexture : public DSTexture {
	public:
		DSIndexedTexture();
		DSIndexedTexture(nv::vec2i const&);
		DSIndexedTexture(Image*);
		DSIndexedTexture(DSIndexedTexture const&);
		DSIndexedTexture(nv::vec2i const&, nv::vec2i const&, DSTexture const&);
		DSIndexedTexture(nv::vec2i const&, nv::vec2i const&, DSIndexedTexture const&);

		std::vector<std::vector<nv::vec2i*>> const& getPixelLocations() const;
		void setPixelLocation(int, int, nv::vec2i const&);
		
		void createBlankImageIndex();
		void doubleSize(DSCoherentTexture const&);

	private:
		std::vector<std::vector<nv::vec2i*>>	_pixelLocations;
		DSTexture*								_inputTexture;
};
