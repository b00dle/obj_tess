#ifndef DSTexture_H
#define DSTexture_H

#include "Image.h"

#include "sdk_swak.h"
#include "FreeImage.h"
#include <vector>
#include <map>

class DSTexture {
	public:
		~DSTexture();
        DSTexture();
		DSTexture(nv::vec2ui);
        DSTexture(Image*);
        DSTexture(nv::vec2ui, nv::vec2ui, DSTexture&);

		void				clearPixelValues();
        void				saveToFile(FREE_IMAGE_FORMAT, const char*);
                       
        bool				isInBounds(nv::vec2i const&) const;
        nv::vec2i const		mirrorNotInBounds(nv::vec2i const&) const;
        nv::vec2i const		wrapNotInBounds(nv::vec2i const&) const;
        		
        int					euclideanDistanceSquared(DSTexture const&) const;
        int					euclideanDistanceSquared(nv::vec2ui const&, DSTexture const&, nv::vec2ui const&) const;
		int					euclideanDistanceSquaredNeighbourhood(nv::vec2ui const&, DSTexture const&, nv::vec2ui const&, int) const;
        int					euclideanDistanceSquaredNeighbourhoodWrap(nv::vec2ui const&, DSTexture const&, nv::vec2ui const&, int) const;
        
		nv::vec2ui const	findClosestNeighbourhood(DSTexture const&, nv::vec2ui&, int) const;
        void				halfSize();
		
	protected:
		std::vector<std::vector<nv::vec3f*>>    _pixels;
		Image*									_image;
		nv::vec2ui								_size;
};


#endif