#ifndef IMAGE_H
#define IMAGE_H

#include <sstream>
#include <vector>
#include "FreeImage.h"
#include "sdk_swak.h"

class Image {
	public:
		Image(const char*);
		~Image();
		
		unsigned int		getWidth()			const;
		unsigned int		getHeight()			const;
		nv::vec3f const&	getPixel(int, int)	const;
		unsigned			getTexBufferID()	const;

		void				setPixel(int, int, nv::vec3f const&);
		void				setPixel(int, int, float, float, float);
		void				setTexBuffer(unsigned);
		bool				scale(unsigned int, unsigned int);

		bool				saveToFile(FREE_IMAGE_FORMAT, const char*);

	private:
		bool init();
		bool storePixelValues();
		bool updateBitmap();
		bool createTexBuffer();
		
	private:
		const char*	_filepath;
		FIBITMAP*	_bitmap;
		
		unsigned int _width;
		unsigned int _height;
		unsigned int _pitch;
		
		std::vector<std::vector<nv::vec3f*>> _pixelValues;

		unsigned _tBufferID[1];
		bool	 _doneTBufferInit;
};

#endif