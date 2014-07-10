#include <iostream>
#include "DSTexture.h"

/////////////////////CONSTRUCTORS////////////////////
DSTexture::~DSTexture(){
}

DSTexture::DSTexture():
	_size(32,32),
	_pixels(),
	_image(nullptr)
{
	_pixels.resize(_size.x);
	for(unsigned int column = 0; column < _size.x; ++column){
		_pixels[column].resize(_size.y, nullptr);
		for(auto p : _pixels[column])
			p = new nv::vec3f(0.0f,0.0f,0.0f);
	}
}

DSTexture::DSTexture(nv::vec2i dimensions):
	_size(dimensions.x, dimensions.y),
	_pixels(),
	_image(nullptr)
{
    _pixels.resize(_size.x);
	for(unsigned int column = 0; column < _size.x; ++column){
		_pixels[column].resize(_size.y, nullptr);
		for(auto p : _pixels[column])
			p = new nv::vec3f(0.0f,0.0f,0.0f);
	}
}

DSTexture::DSTexture(Image* imageInput):
	_size(imageInput->getWidth(), imageInput->getHeight()),
	_pixels(),
	_image(imageInput)
{
	_pixels.resize(_size.x);
	for(unsigned int column = 0; column < _size.x; ++column) {
		_pixels[column].resize(_size.y, nullptr);
		for(unsigned int row = 0; row < _pixels[column].size(); ++row)
			_pixels[column][row] = new nv::vec3f(_image->getPixel(column,row));
	}        
}

DSTexture::DSTexture(nv::vec2i startPoint, nv::vec2i dimensions, DSTexture const& input):
	_size(dimensions.x, dimensions.y),
	_pixels(),
	_image(nullptr)
{
    _pixels.resize(_size.x);
	for(unsigned int column = 0; column < _size.x; ++column) {
		_pixels[column].resize(_size.y, nullptr);
		for(int row = 0; row < _pixels[column].size(); ++row)
			_pixels[column][row] = new nv::vec3f(0.0f,0.0f,0.0f);
	}
	
	for(unsigned int row = 0; row < _size.y ; ++row) {
        for(unsigned int column = 0; column < _size.x; ++column) {
            nv::vec3f pixel = *(input._pixels[(unsigned int)startPoint.x+column][(unsigned int)startPoint.y+row]);
			*(_pixels[column][row]) = pixel;
        }
    }
}

//////////////////////////////////////////////////////////////////

nv::vec2i const& DSTexture::getSize() const {
	return _size;
}

std::vector<std::vector<nv::vec3f*>> const& DSTexture::getPixels() const {
	return _pixels;
}

Image const DSTexture::getImage() const {
	return *_image;
}

void DSTexture::setPixel(int x, int y, nv::vec3f const& value) {
	delete _pixels[x][y];
	_pixels[x][y] = new nv::vec3f(value);
}

void DSTexture::clearPixelValues() {
	for(auto col: _pixels){
		for(auto row: col)
			*row = nv::vec3f(0.0,0.0,0.0);
	}
}

void DSTexture::saveToFile(FREE_IMAGE_FORMAT format, const char* filepath) {
	//TODO freeimage create image
	FreeImage_Initialise();

	FIBITMAP* bitmap = FreeImage_Allocate(_size.x,_size.y, 32); //32 bit color depth
	bitmap = FreeImage_ConvertToRGBF(bitmap);

	if(FreeImage_GetImageType(bitmap) != FIT_RGBF)
		std::cout << "failed to convert image to RGBF format\n";
	
	unsigned int pitch  = FreeImage_GetPitch(bitmap);

	BYTE *bits = (BYTE*)FreeImage_GetBits(bitmap);
	for(unsigned int y = 0; y < _size.y; y++) {
		FIRGBF *pixel = (FIRGBF*)bits;
		for(unsigned int x = 0; x < _size.x; x++) {
			pixel[x].red	= _pixels[x][y]->x;
			pixel[x].green	= _pixels[x][y]->y;
			pixel[x].blue	= _pixels[x][y]->z;	
		}
		// next line
		bits += pitch;
	}

	FIBITMAP* b = FreeImage_ToneMapping(bitmap, FITMO_REINHARD05); //convert RGBF to bitmap
	FreeImage_AdjustColors(b, -5.0, 5.0, 0.45);

	if(FreeImage_Save(format, b, filepath))
		std::cout << "Successfully stored texture at " << filepath << std::endl;
	else
		std::cout << "Error storing texture at " << filepath << std::endl;
}

bool DSTexture::isInBounds(nv::vec2i const& point) const {
	return point.x > 0 && point.y > 0 && point.x < _size.x && point.y < _size.y;  
}

nv::vec2i const DSTexture::mirrorNotInBounds(nv::vec2i const& point) const {
	nv::vec2i temp = point;
	
	if(point.x < 0)
        temp.x = -temp.x;
    else if(point.x >= _size.x)
        temp.x = _size.x - 1 + (_size.x - 1 - temp.x);

    if(point.y < 0)
        temp.y = -temp.y;
    else if(point.y >= _size.y)
        temp.y = _size.y - 1 + (_size.y - 1 - temp.y);

	return temp;
}

nv::vec2i const DSTexture::wrapNotInBounds(nv::vec2i const& point) const {
	nv::vec2i temp = point;

	if(point.x < 0)
        temp.x = _size.x + temp.x;
    else if(point.x >= _size.x)
        temp.x = (temp.x - (_size.x - 1));
    
	if(point.y < 0)
        temp.y = _size.y + temp.y;
    else if(point.y >= _size.y)
        temp.y = (temp.y - (_size.y - 1));
    
    return temp;
}

int DSTexture::euclideanDistanceSquared(DSTexture const& tex) const {
	int red = 0, blue = 0, green = 0;
	int numPixels = _size.x * _size.y;

	for(unsigned int x = 0; x < _size.x; ++x){
		for(unsigned int y = 0; y < _size.y; ++y){
			int distRed		= (int) ( 255 * (_pixels[x][y]->x - tex._pixels[x][y]->r) );
			int distGreen	= (int) ( 255 * (_pixels[x][y]->y - tex._pixels[x][y]->y) );
			int distBlue	= (int) ( 255 * (_pixels[x][y]->z - tex._pixels[x][y]->z) );

			red		+= distRed * distRed;
			green	+= distGreen * distGreen;
			blue	+= distBlue * distBlue;
		}
	}

	red		/= numPixels;
	green	/= numPixels;
	blue	/= numPixels;

	return red + blue + green;
}

int DSTexture::euclideanDistanceSquared(nv::vec2i const& inPixel, DSTexture const& compTexture, nv::vec2i const& compPixel) const {
	int red = 0, green = 0, blue = 0;

	int distRed		= (int) ( 255 * (_pixels[inPixel.x][inPixel.y]->x - compTexture._pixels[compPixel.x][compPixel.y]->x) );
	int distGreen	= (int) ( 255 * (_pixels[inPixel.x][inPixel.y]->y - compTexture._pixels[compPixel.x][compPixel.y]->y) );
	int distBlue	= (int) ( 255 * (_pixels[inPixel.x][inPixel.y]->z - compTexture._pixels[compPixel.x][compPixel.y]->z) );

	red		+= distRed * distRed;
	green	+= distGreen * distGreen;
	blue	+= distBlue * distBlue;

	return red + blue + green;
}

int DSTexture::euclideanDistanceSquaredNeighbourhood(nv::vec2i const& inPoint, DSTexture const& compTexture, nv::vec2i const& compPoint, int neighbourSize) const {
	int red = 0, green = 0, blue = 0;
	int numPixels = 0;

	nv::vec2i inPos;
	nv::vec2i compPos;

	for(int x = -neighbourSize; x <= neighbourSize; ++x) {
		for(int y = -neighbourSize; y <= neighbourSize; ++y) {
			inPos.x = x + inPoint.x;
			inPos.y = y + inPoint.y;
			inPos = mirrorNotInBounds(inPos);

			compPos.x = x + compPoint.x;
			compPos.y = y + compPoint.y;
			compPos = compTexture.mirrorNotInBounds(compPos);

			if(isInBounds(inPos) && compTexture.isInBounds(compPos)) {
				int distRed		= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->x -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->x) );
				int distGreen	= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->y -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->y) );
				int distBlue	= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->z -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->z) );
				red		+= distRed * distRed;
				green	+= distGreen * distGreen;
				blue	+= distBlue * distBlue;
				++numPixels;
			}
		}
	}

	red		/= numPixels;
	green	/= numPixels;
	blue	/= numPixels;
	return red + green + blue;
}

int DSTexture::euclideanDistanceSquaredNeighbourhoodWrap(nv::vec2i const& inPoint, DSTexture const& compTexture, nv::vec2i const& compPoint, int neighbourSize) const {
	int red = 0, green = 0, blue = 0;
	int numPixels = 0;

	nv::vec2i inPos;
	nv::vec2i compPos;

	for(int x = -neighbourSize; x <= neighbourSize; ++x) {
		for(int y = -neighbourSize; y <= neighbourSize; ++y) {
			inPos.x = x + inPoint.x;
			inPos.y = y + inPoint.y;
			inPos = mirrorNotInBounds(inPos);

			compPos.x = x + compPoint.x;
			compPos.y = y + compPoint.y;
			compPos = compTexture.wrapNotInBounds(compPos);

			if(isInBounds(inPos) && compTexture.isInBounds(compPos)) {
				int distRed		= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->x -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->x) );
				int distGreen	= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->y -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->y) );
				int distBlue	= (int) ( 255 * (_pixels[(int)inPos.x][(int)inPos.y]->z -  compTexture._pixels[(int)compPos.x][(int)compPos.y]->z) );
				red		+= distRed * distRed;
				green	+= distGreen * distGreen;
				blue	+= distBlue * distBlue;
				++numPixels;
			}
		}
	}

	red		/= numPixels;
	green	/= numPixels;
	blue	/= numPixels;
	return red + green + blue;
}

nv::vec2i const DSTexture::findClosestNeighbourhood(DSTexture const& compTexture, nv::vec2i& compPoint, int neighbourSize) const {
	nv::vec2i closestPoint;
	int closestDist = 10000000;

	nv::vec2i currentPoint;
	int currentDist = 0;
	unsigned int neighbourIncrement = (unsigned int) ( (2*neighbourSize + 1) / 4 );

	for(unsigned int x = 0; x < _size.x; x += neighbourIncrement) {
		for(unsigned int y = 0; y < _size.y; y += neighbourIncrement) {
			currentPoint.x = x;
			currentPoint.y = y;
			currentDist = euclideanDistanceSquaredNeighbourhood(currentPoint, compTexture, compPoint, neighbourSize);
			if(currentDist < closestDist) {
				closestDist = currentDist;
				closestPoint = currentPoint;
			}
		}
	}

	return closestPoint;
}

void DSTexture::halfSize() {
	nv::vec2i sizeTemp = _size;
	sizeTemp = (nv::vec2i) sizeTemp/2;

	std::vector<std::vector<nv::vec3f*>> pixelsTemp;
	pixelsTemp.resize(sizeTemp.x);
	for(unsigned int column = 0; column < sizeTemp.x; ++column) {
		pixelsTemp[column].resize(sizeTemp.y, nullptr);
		for(unsigned int row = 0; row < sizeTemp.y; ++row)
			pixelsTemp[column][row] = new nv::vec3f(0.0f,0.0f,0.0f);
	}

	for(unsigned int y = 0; y < sizeTemp.y; ++y) {
		for(unsigned int x = 0; x < sizeTemp.x; ++x) {
			unsigned int r = (unsigned int) (255 * _pixels[x][y]->x);
			unsigned int g = (unsigned int) (255 * _pixels[x][y]->y);
			unsigned int b = (unsigned int) (255 * _pixels[x][y]->z);
			unsigned int count = 1;
			if(x+1 < _size.x) {
				r += (unsigned int) (255 * _pixels[x+1][y]->x);
				g += (unsigned int) (255 * _pixels[x+1][y]->y);
				b += (unsigned int) (255 * _pixels[x+1][y]->z);
				++count;
			}
			if(y+1 < _size.y) {
				r += (unsigned int) (255 * _pixels[x][y+1]->x);
				g += (unsigned int) (255 * _pixels[x][y+1]->y);
				b += (unsigned int) (255 * _pixels[x][y+1]->z);
				++count;
			}
			if(y+1 < _size.y && x+1 < _size.x) {
				r += (unsigned int) (255 * _pixels[x+1][y+1]->x);
				g += (unsigned int) (255 * _pixels[x+1][y+1]->y);
				b += (unsigned int) (255 * _pixels[x+1][y+1]->z);
				++count;
			}
			r /= count;
			g /= count;
			b /= count;
			pixelsTemp[x][y]->x = (float) r / 255.0f; 
			pixelsTemp[x][y]->y = (float) g / 255.0f; 
			pixelsTemp[x][y]->z = (float) b / 255.0f; 
		}
	}
	
	for(auto row: _pixels)
		row.clear();
	_pixels.clear();

	_pixels = pixelsTemp;
	_size = sizeTemp;
	_image->scale(_size.x, _size.y);
}

void DSTexture::scaleImage(unsigned int w, unsigned int h) {
	_image->scale(w,h);
}
        