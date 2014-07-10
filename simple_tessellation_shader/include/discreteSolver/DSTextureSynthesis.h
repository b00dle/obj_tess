#pragma once

#include "Image.h"
#include "sdk_swak.h"
#include "DSTexture.h"
#include "DSIndexedTexture.h"
#include "DSCoherentTexture.h"
#include <vector>
#include <string>

class DSTextureSynthesis {
	public:
		DSTextureSynthesis();
		~DSTextureSynthesis();

		void loadImage(std::string const&);
		void synthesize();
		void synthesize(nv::vec2i const&, int);

	private:
		nv::vec3f const				featheredPixel(nv::vec3f const&, nv::vec3f const&, float);
		DSTexture const				createRandomTexture(DSTexture&, std::vector<DSTexture*> const&, nv::vec2i const&, int);
		DSIndexedTexture const		createRandomTexture(DSIndexedTexture&, std::vector<DSIndexedTexture*> const&, nv::vec2i const&);

		DSTexture const				discreetOptimization(DSTexture const&, DSTexture const&, int);
		DSIndexedTexture const		discreetOptimization(DSIndexedTexture const&, DSCoherentTexture &, int, int);

		Image const					scaleImage(nv::vec2i const&, Image const&);

	private:
		std::vector<DSCoherentTexture*>	_originalTextures;
		std::vector<DSIndexedTexture*>	_synthesizedTextures;
		Image*							_originalImage;
		int								_quality;
		std::vector<DSIndexedTexture*>	_patchListIndexed;
		DSIndexedTexture*				_patchedTexture;
		std::string						_imagePath;
		
};