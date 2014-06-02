#ifndef IMAGE_H
#define IMAGE_H

#include <sstream>
#include <vector>
#include "FreeImage.h"

struct Color {
	float r;
	float g;
	float b;
	
	std::string const print() {
		std::ostringstream ss;
		ss << "(" << r << " | " << g << " | " << b << ")";
		return ss.str();
	}
};

class Image {
	public:
		Image(const char*);
		~Image();

		Color const&	getPixel(int, int) const;

		void			setPixel(int, int, Color const&);
		void			setPixel(int, int, float, float, float);

	private:
		bool init();
		bool storePixelValues();
		
	private:
		const char*	_filepath;
		FIBITMAP*	_bitmap;
		
		unsigned int _width;
		unsigned int _height;
		unsigned int _pitch;

		std::vector<std::vector<Color*>> _pixelValues;
};

#endif