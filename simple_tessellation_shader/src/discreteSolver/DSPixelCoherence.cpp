#include "DSPixelCoherence.h"
#include <queue>

DSPixelCoherence::DSPixelCoherence():
	_neighbourSizes(),
	_neighbourPos(),
	_neighbourPosDim(11),
	_pixelPos()
{
	nv::vec2i* tempVec2ui = new nv::vec2i();
	std::vector<nv::vec2i*> tempVector;
	tempVector.push_back(tempVec2ui);
	_neighbourPos.push_back(tempVector);
}

DSPixelCoherence::DSPixelCoherence(DSCoherentTexture const& sampleTex, nv::vec2i const& pos, std::vector<int> const& neighbourSizes):
	_neighbourSizes(neighbourSizes),
	_neighbourPos(),
	_neighbourPosDim(11),
	_pixelPos(pos)
{
	// creates an array of 11 coherent pixels for each pixel for each neighbourhood size
	_neighbourPos.resize(_neighbourSizes.size());
	for(int x = 0; x < _neighbourPos.size(); ++x) {
		_neighbourPos[x].resize(_neighbourPosDim, nullptr);
		for(int y = 0; y < _neighbourPos[x].size(); ++y)
			_neighbourPos[x][y] = new nv::vec2i(0,0);
	}

	// loop through each neighbourhood size
	for(int i = 0; i < _neighbourSizes.size(); ++i) {
		std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> pQueue;

		// incerementing according to the size of the neighbourhood
		int incrementValue = ceil( (float)(_neighbourSizes[i] / 4.0f) );
		incrementValue = 1; //WTF!?!?!?
		int start = 0;
		int endX = sampleTex.getSize().x;
		int endY = sampleTex.getSize().y;

		// find neighbourhood distance to each pixel
		for(unsigned int x = start; x < endX; x += incrementValue) {
			for(unsigned int y = start; y < endY; y += incrementValue) {
				int dist = sampleTex.euclideanDistanceSquaredNeighbourhood(_pixelPos, sampleTex, nv::vec2i(x,y), _neighbourSizes[i]);
				DSPixel p(nv::vec2i(x,y), dist);
				pQueue.push(p);
			}
		}

		for(int t = 0; t < _neighbourPosDim; ++t) {
			_neighbourPos[i][t] = new nv::vec2i(pQueue.top().position);
			pQueue.pop();
		}
	}
}

DSPixelCoherence::DSPixelCoherence(DSCoherentTexture const& sampleTex, DSPixelCoherence const& lowResCo, nv::vec2i const& pos, std::vector<int> const& neighbourSizes):
	_neighbourSizes(neighbourSizes),
	_neighbourPos(),
	_neighbourPosDim(11),
	_pixelPos(pos)
{
	// creates an array of 11 coherent pixels for each pixel for each neighbourhood size
	_neighbourPos.resize(_neighbourSizes.size());
	for(int x = 0; x < _neighbourPos.size(); ++x) {
		_neighbourPos[x].resize(_neighbourPosDim, nullptr);
		for(int y = 0; y < _neighbourPos[x].size(); ++y)
			_neighbourPos[x][y] = new nv::vec2i(0,0);
	}
	
	std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> pQueue;
	//increment according to an the size of the neighbourhood     
    //find the neighbourhood distance to each pixel        
    for(int i = 0; i < _neighbourSizes.size(); ++i) {
		int index = i-1;
		if(index < 0)
			index = 0;

		for(int t = 0; t < lowResCo._neighbourPosDim; ++t) {
			nv::vec2i coherencePoint(*(lowResCo._neighbourPos[index][t]));
			for(unsigned int x = 0; x < 2; ++x) {
				for(unsigned int y = 0; y < 2; ++y) {
					nv::vec2i closestPoint(coherencePoint);
					closestPoint *= 2;
					closestPoint.x += x;
					closestPoint.y += y;
					if( sampleTex.isInBounds(nv::vec2i(closestPoint.x,closestPoint.y)) ) {
						int dist = sampleTex.euclideanDistanceSquaredNeighbourhood(_pixelPos, sampleTex, closestPoint, _neighbourSizes[i]);
						DSPixel p(closestPoint,dist);
						pQueue.push(p);
					}
				}
			}
		}

		for(int t = 0; t < _neighbourPosDim; ++t) {
			_neighbourPos[i][t] = new nv::vec2i(pQueue.top().position);
			pQueue.pop();
		}
	}
}

std::vector<std::vector<nv::vec2i*>> const& DSPixelCoherence::getNeighbourPos() const {
	return _neighbourPos;
}