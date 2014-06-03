#include "Image.h"

#include <iostream>

Image::Image(const char* filepath):
	_filepath(filepath),
	_bitmap(nullptr),
	_width(0),
	_height(0),
	_pitch(0),
	_pixelValues()
{
	if(init()) {
		std::cout << "loaded image: " << _filepath << "\n";
		std::cout << "image size: width=" << _width << " height=" << _height << "\n";
	}
}

Image::~Image()
{}

unsigned int Image::getWidth() const {
	return _width;
}

unsigned int Image::getHeight() const {
	return _height;
}

nv::vec3f const& Image::getPixel(int x, int y) const{
	return *(_pixelValues[x][y]);
}

void Image::setPixel(int x, int y, nv::vec3f const& c){
	_pixelValues[x][y]->x = c.x;
	_pixelValues[x][y]->y = c.y;
	_pixelValues[x][y]->z = c.z;
}

void Image::setPixel(int x, int y, float r, float g, float b){
	_pixelValues[x][y]->x = r;
	_pixelValues[x][y]->y = g;
	_pixelValues[x][y]->z = b;
}

bool Image::saveToFile(FREE_IMAGE_FORMAT format, const char* filepath) {
	if(updateBitmap()) {
		FIBITMAP* _b = FreeImage_ToneMapping(_bitmap, FITMO_REINHARD05); //convert RGBF to bitmap
		FreeImage_AdjustColors(_b, -5.0, 5.0, 0.45);
		return FreeImage_Save(format, _b, filepath);
	}
	return false;
}

bool Image::init() {
	FreeImage_Initialise();
	
	_bitmap = FreeImage_Load(FreeImage_GetFileType(_filepath, 0),
							 _filepath);
	
	if(!_bitmap){
		std::cout << "image: \'" << _filepath << "\' didn't load\n";
		return false;
	}
	
	_bitmap = FreeImage_ConvertToRGBF(_bitmap);
	
	if(!_bitmap){
		std::cout << "error converting image: \'" << _filepath << "\' to RGBF format.\n";
		return false;
	}

	_width  = FreeImage_GetWidth(_bitmap);
	_height = FreeImage_GetHeight(_bitmap);
	_pitch  = FreeImage_GetPitch(_bitmap);

	if(!storePixelValues()){
		std::cout << "error storing pixel values for file: " << _filepath << "\n";
		return false;
	}

	return true;
}

bool Image::storePixelValues() {
	_pixelValues.resize(_width);
	for(unsigned int column = 0; column < _width; ++column)
		_pixelValues[column].resize(_height, nullptr);
	
	if(FreeImage_GetImageType(_bitmap) != FIT_RGBF)
		return false;

	BYTE *bits = (BYTE*)FreeImage_GetBits(_bitmap);
	for(unsigned int y = 0; y < _height; y++) {
		FIRGBF *pixel = (FIRGBF*)bits;
		for(unsigned int x = 0; x < _width; x++) {
			float W = pixel[x].red + pixel[x].green + pixel[x].blue;
			if(W > 0) {
				_pixelValues[x][y] = new nv::vec3f();
				_pixelValues[x][y]->x = pixel[x].red;
				_pixelValues[x][y]->y = pixel[x].green;
				_pixelValues[x][y]->z = pixel[x].blue;	
			} 
			else {
				_pixelValues[x][y] = new nv::vec3f();
				_pixelValues[x][y]->x = 0.0f;
				_pixelValues[x][y]->y = 0.0f;
				_pixelValues[x][y]->z = 0.0f;
			}
		}
		// next line
		bits += _pitch;
	}

	return true;
}

bool Image::updateBitmap() {

	if(FreeImage_GetImageType(_bitmap) != FIT_RGBF)
		return false;

	BYTE *bits = (BYTE*)FreeImage_GetBits(_bitmap);
	for(unsigned int y = 0; y < _height; y++) {
		FIRGBF *pixel = (FIRGBF*)bits;
		for(unsigned int x = 0; x < _width; x++) {
			pixel[x].red	= _pixelValues[x][y]->x;
			pixel[x].green	= _pixelValues[x][y]->y;
			pixel[x].blue	= _pixelValues[x][y]->z;	
		}
		// next line
		bits += _pitch;
	}

	return true;
}