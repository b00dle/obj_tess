#pragma once

#include "Image.h"

#include "sdk_swak.h"
#include "FreeImage.h"
#include <vector>
#include <map>

class DSTexture {
	public:
		~DSTexture();
        DSTexture();
		DSTexture(nv::vec2i);
        DSTexture(Image*);
        DSTexture(nv::vec2i, nv::vec2i, DSTexture const&);

		nv::vec2i const&	getSize() const;
		std::vector<std::vector<nv::vec3f*>> const& getPixels() const;
		Image const			getImage() const;

		void				setPixel(int, int, nv::vec3f const&);

		void				clearPixelValues();
        void				saveToFile(FREE_IMAGE_FORMAT, const char*);
                       
        bool				isInBounds(nv::vec2i const&) const;
        nv::vec2i const		mirrorNotInBounds(nv::vec2i const&) const;
        nv::vec2i const		wrapNotInBounds(nv::vec2i const&) const;
        		
        int					euclideanDistanceSquared(DSTexture const&) const;
        int					euclideanDistanceSquared(nv::vec2i const&, DSTexture const&, nv::vec2i const&) const;
		int					euclideanDistanceSquaredNeighbourhood(nv::vec2i const&, DSTexture const&, nv::vec2i const&, int) const;
        int					euclideanDistanceSquaredNeighbourhoodWrap(nv::vec2i const&, DSTexture const&, nv::vec2i const&, int) const;
        
		nv::vec2i const		findClosestNeighbourhood(DSTexture const&, nv::vec2i&, int) const;
        void				halfSize();
		void				scaleImage(unsigned int, unsigned int);

		
	protected:
		std::vector<std::vector<nv::vec3f*>>    _pixels;
		Image*									_image;
		nv::vec2i								_size;
};