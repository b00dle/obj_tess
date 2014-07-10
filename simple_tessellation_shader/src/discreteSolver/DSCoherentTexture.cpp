#include "DSCoherentTexture.h"
#include <omp.h>

DSCoherentTexture::DSCoherentTexture():
	DSTexture(),
	_pixelCoherences(),
	_neighbourSizes()
{
	createCoherence();
}

DSCoherentTexture::DSCoherentTexture(nv::vec2i const& dimensions):
	DSTexture(dimensions),
	_pixelCoherences(),
	_neighbourSizes()
{
	createCoherence();
}

DSCoherentTexture::DSCoherentTexture(Image* imageInput):
	DSTexture(imageInput),
	_pixelCoherences(),
	_neighbourSizes()
{
	createCoherence();
}

DSCoherentTexture::DSCoherentTexture(nv::vec2i const& startPoint, nv::vec2i const& dimensions, DSTexture const& patch):
	DSTexture(startPoint,dimensions,patch),
	_pixelCoherences(),
	_neighbourSizes()
{
	createCoherence();
}

DSCoherentTexture::DSCoherentTexture(Image* imageInput, DSCoherentTexture const& lowResTex):
	DSTexture(imageInput),
	_pixelCoherences(),
	_neighbourSizes()
{
	_pixelCoherences.resize(_size.x);
	for(unsigned int x = 0; x < _size.x; ++x) {
		_pixelCoherences[x].resize(_size.y, nullptr);
	}

	int halfSize = 4;
	if(_size.x/2 < halfSize*2+1) {
		_neighbourSizes.push_back(halfSize);
	}
	else {
		int largest = halfSize;
		int secondLargest = halfSize;

		for(int i = halfSize*2+1; i <= _size.x/2; halfSize *= 2, i = halfSize*2+1) {
			secondLargest = largest;
			largest = halfSize;
		}

		_neighbourSizes.push_back(secondLargest);
		_neighbourSizes.push_back(largest);
	}

	#pragma omp parallel for num_threads(8)
	for(unsigned int x = 0; x < _size.x; ++x) {
		for(unsigned int y = 0; y < _size.y; ++y) {
			unsigned int xHalf = floor(x/2);
			unsigned int yHalf = floor(y/2);
			
			if(floor(x/2) >= lowResTex._size.x)
				--xHalf;
			if(floor(y/2) >= lowResTex._size.y)
				--yHalf;

			_pixelCoherences[x][y] = new DSPixelCoherence(*this, *lowResTex._pixelCoherences[xHalf][yHalf], nv::vec2i(x,y), _neighbourSizes);
		}
	}
}

int DSCoherentTexture::getNeighbourSize(int i) const {
	return _neighbourSizes[i];
}

std::vector<int> const& DSCoherentTexture::getNeighbourSizes() const {
	return _neighbourSizes;
}

void DSCoherentTexture::createCoherence() {
	_pixelCoherences.resize(_size.x);
	for(unsigned int x = 0; x < _size.x; ++x) {
		_pixelCoherences[x].resize(_size.y, nullptr);
	}

	int halfSize = 4;
	if(_size.x/2 < halfSize*2+1) {
		_neighbourSizes.push_back(halfSize);
	}
	else {
		int largest = halfSize;
		int secondLargest = halfSize;

		for(int i = halfSize*2+1; i <= _size.x/2; halfSize *= 2, i = halfSize*2+1) {
			secondLargest = largest;
			largest = halfSize;
		}

		_neighbourSizes.push_back(secondLargest);
		_neighbourSizes.push_back(largest);
	}

	#pragma omp parallel for num_threads(8)
	for(unsigned int x = 0; x < _size.x; ++x) {
		for(unsigned int y = 0; y < _size.y; ++y) {
			_pixelCoherences[x][y] = new DSPixelCoherence(*this, nv::vec2i(x,y), _neighbourSizes);
		}
	}
}

void DSCoherentTexture::findCandidates(DSIndexedTexture const& compTexture, nv::vec2i const & compPoint, int neighbourSizeIndex, int coherenceQuality, std::vector<nv::vec2i*>& candidates) {
	// the set of 11 closest neighbourhoods, created from the pixels neighbourhood coherence

	//loop through the neighbour
	int nSize = _neighbourSizes[neighbourSizeIndex];
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= -1; ++y) {
			nv::vec2i refPoint(compPoint.x, compPoint.y);
			refPoint.x += x;
			refPoint.y += y;

			//the next point, temporarily holds the coherence point
            nv::vec2i nextPoint;
            //mirror the point if its not in bounds
			refPoint = compTexture.wrapNotInBounds(refPoint);
            //get the sample texture index from the indexedtexture, according to the reference point
			nv::vec2i inputPoint = *compTexture.getPixelLocations()[refPoint.x][refPoint.y];
			//loop through each coherent point of the reference point and add them to the candidate list.
			for(int j = 0; j < coherenceQuality; ++j) {
				//gets the point
				nextPoint = *(_pixelCoherences[inputPoint.x][inputPoint.y]->getNeighbourPos()[neighbourSizeIndex][j]);

				//offsets the point to be in the realtive position from the reference point
				nextPoint.x -= x;
				nextPoint.y -= y;
				nextPoint = mirrorNotInBounds(nextPoint);
				candidates.push_back(new nv::vec2i(nextPoint));
			}
		}
	}
}

std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> DSCoherentTexture::findClosestNeighbourhood(DSIndexedTexture const& compTexture, nv::vec2i const& compPoint, int neighbourSizeIndex, int coherenceQuality) {
	// the overloaded method for finding the closest neighborhood with k-coherence
    // the set of 11 closest neighborhoods, created from the pixels neighborhood coherence
    std::vector<nv::vec2i> candidateSet;
    // loop through the neighborhood
            // i dont think its necessary to go through the whole neighborhood, just the immediately surrounding pixels
    
	int nSize = _neighbourSizes[neighbourSizeIndex];
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			// the reference point to the next pixel in the neighborhood
            nv::vec2i refPoint = compPoint;
			refPoint.x += x;
			refPoint.y += y;

			// the next point
			nv::vec2i nextPoint;
			if(compTexture.isInBounds(refPoint)) {
				// get sample texture index from indexedtexture, according to ref point
				nv::vec2i inputPoint = *compTexture.getPixelLocations()[refPoint.x][refPoint.y];

				// loop through each coherent point of ref and add to candidate list
				for(int j = 0; j < coherenceQuality; ++j) {
					// gets the point
					nextPoint = *_pixelCoherences[inputPoint.x][inputPoint.y]->getNeighbourPos()[neighbourSizeIndex][j];
					// offsets point to relative position from ref point
					nextPoint.x -= x;
					nextPoint.y -= y;
					if(isInBounds(nextPoint))
						candidateSet.push_back(nextPoint);
				}
			}
		}
	}

	DSPixel closest;
	closest.distance = 1000000;
	std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> pQueue;

	nv::vec2i current;
	int currentDist = 0;

	//loop through candidate set
	for(int x = 0; x < candidateSet.size(); ++x) {
		current = candidateSet[x];

		//get neighbourhood distance
		currentDist = euclideanDistanceSquaredNeighbourhood(current, compTexture, compPoint, _neighbourSizes[neighbourSizeIndex]);
		//check if it is closer than current model
		closest.distance = currentDist;
		closest.position.x = current.x;
		closest.position.y = current.y;
		pQueue.push(closest);
	}

	return pQueue;
}