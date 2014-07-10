#include "DSIndexedTexture.h"

DSIndexedTexture::DSIndexedTexture():
	DSTexture(),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	createBlankImageIndex();
}

DSIndexedTexture::DSIndexedTexture(nv::vec2i const& dimensions):
	DSTexture(dimensions),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	createBlankImageIndex();
}

DSIndexedTexture::DSIndexedTexture(Image* image):
	DSTexture(image),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	_pixelLocations.resize(_size.x);
	for(unsigned int x = 0; x < _pixelLocations.size(); ++x){
		_pixelLocations[x].resize(_size.y, nullptr);
		for(unsigned int y = 0; y < _pixelLocations[x].size(); ++y)
			_pixelLocations[x][y] = new nv::vec2i(x,y);
	}
}

DSIndexedTexture::DSIndexedTexture(DSIndexedTexture const& indexedTex):
	DSTexture(indexedTex._image),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	_pixelLocations.resize(_size.x);
	for(unsigned int x = 0; x < _pixelLocations.size(); ++x){
		_pixelLocations[x].resize(_size.y, nullptr);
		for(unsigned int y = 0; y < _pixelLocations[x].size(); ++y)
			_pixelLocations[x][y] = new nv::vec2i(x,y);
	}
}

DSIndexedTexture::DSIndexedTexture(nv::vec2i const& startPoint, nv::vec2i const& dimensions, DSTexture const& input):
	DSTexture(startPoint, dimensions, input),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	_pixelLocations.resize(_size.x);
	for(unsigned int x = 0; x < _pixelLocations.size(); ++x){
		_pixelLocations[x].resize(_size.y, nullptr);
		for(unsigned int y = 0; y < _pixelLocations[x].size(); ++y)
			_pixelLocations[x][y] = new nv::vec2i(startPoint.x + x, startPoint.y + y);
	}
}

DSIndexedTexture::DSIndexedTexture(nv::vec2i const& startPoint, nv::vec2i const& dimensions, DSIndexedTexture const& input):
	DSTexture(startPoint, dimensions, input),
	_pixelLocations(),
	_inputTexture(nullptr)
{
	_pixelLocations.resize(_size.x);
	for(unsigned int x = 0; x < _pixelLocations.size(); ++x){
		_pixelLocations[x].resize(_size.y, nullptr);
		for(unsigned int y = 0; y < _pixelLocations[x].size(); ++y)
			_pixelLocations[x][y] = new nv::vec2i(startPoint.x + x, startPoint.y + y);
	}
}

std::vector<std::vector<nv::vec2i*>> const& DSIndexedTexture::getPixelLocations() const {
	return _pixelLocations;
}

void DSIndexedTexture::setPixelLocation(int x, int y, nv::vec2i const& value) {
	delete _pixelLocations[x][y];
	_pixelLocations[x][y] = new nv::vec2i(value);
}

void DSIndexedTexture::createBlankImageIndex() {
	_pixelLocations.resize(_size.x);
	for(unsigned int x = 0; x < _pixelLocations.size(); ++x){
		_pixelLocations[x].resize(_size.y, nullptr);
		for(unsigned int y = 0; y < _pixelLocations[x].size(); ++y)
			_pixelLocations[x][y] = new nv::vec2i(0,0);
	}
}

void DSIndexedTexture::doubleSize(DSCoherentTexture const& originalImage) {
	nv::vec2i sizeTemp = _size * 2;
	std::vector<std::vector<nv::vec3f*>>	pixelsTemp;
	std::vector<std::vector<nv::vec2i*>>	pixelLocationsTemp;

	pixelsTemp.resize(sizeTemp.x);
	pixelLocationsTemp.resize(sizeTemp.x);
	for(int x = 0; x < sizeTemp.x; ++x) {
		pixelsTemp[x].resize(sizeTemp.y, nullptr);
		pixelLocationsTemp[x].resize(sizeTemp.y, nullptr);
		for(int y = 0; y < sizeTemp.y; ++y) {
			pixelsTemp[x][y]			= new nv::vec3f(0.0f,0.0f,0.0f);
			pixelLocationsTemp[x][y]	= new nv::vec2i(0,0);
		}
	}

	nv::vec2i temp;
	for(int y = 0; y < _size.y; ++y) {
		for(int x = 0; x < _size.x; ++x) {
			for(unsigned int i = 0; i < 2; ++i) {
				for(unsigned int j = 0; j < 2; ++j) {
					temp = *_pixelLocations[x][y];
					nv::vec2i index(temp.x*2+i, temp.y*2+j);
					pixelsTemp[x*2+i][y*2+j] = new nv::vec3f(*originalImage.getPixels()[index.x][index.y]);
					pixelLocationsTemp[x*2+i][y*2+j] = new nv::vec2i(index);
				}
			}
		}
	}

	for(auto row: _pixels)
		row.clear();
	_pixels.clear();

	for(auto row: _pixelLocations)
		row.clear();
	_pixelLocations.clear();

	_pixels = pixelsTemp;
	_pixelLocations = pixelLocationsTemp;
	_size = sizeTemp;

	_image->scale(_size.x, _size.y);
}