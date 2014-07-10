#include "DSTextureSynthesis.h"
#include <time.h>
#include <iostream>
#include <omp.h>
#include <queue>

DSTextureSynthesis::DSTextureSynthesis():
	_originalTextures(),
	_synthesizedTextures(),
	_originalImage(),
	_quality(),
	_patchListIndexed(),
	_patchedTexture(),
	_imagePath()
{}

DSTextureSynthesis::~DSTextureSynthesis()
{}

void DSTextureSynthesis::synthesize(nv::vec2i const& size, int iterations) {
	//========================PATCH IMAGE======================
	// size of synthesized image
	int sizeSquare = _originalTextures[0]->getSize().x;
	
	nv::vec2i sizePatchedImage = size;
	for(int i = 0; i < _originalTextures.size()-1; ++i)
		sizePatchedImage /= 2;

	// amount of overlap for the random texture
	int overlap = 0;

	// size of each patch in random texture
	int sizePatchSquare = sizeSquare / 8;
	nv::vec2i sizePatches(sizePatchSquare, sizePatchSquare);

	// copy image and scale it to synthesized image size
	Image* patchedImage = nullptr;
	_originalTextures[0]->scaleImage((unsigned int) sizePatchedImage.x, (unsigned int) sizePatchedImage.y);
	patchedImage = new Image(_originalTextures[0]->getImage());

	// create and indexed Texture
	_patchedTexture = new DSIndexedTexture(patchedImage);

	// clear the pixels
	_patchedTexture->clearPixelValues();

	// create amount of patches needed
	float numPatchesX = 1 + (float)(sizePatchedImage.x - sizePatches.x) / (float)(sizePatches.x - overlap);
	float numPatchesY = 1 + (float)(sizePatchedImage.y - sizePatches.y) / (float)(sizePatches.y - overlap);

	// create vector from amount of patches needed
	nv::vec2i numPatches((int)ceil(numPatchesX), (int)ceil(numPatchesY));
	
	//========================PATCH LOGIC======================
	// vector of all patches
	_patchListIndexed.resize(numPatches.y*numPatches.x*2);

	// create twice the amount of needed patches, this can be taken down to the required amount
	for(int y = 0; y < numPatches.x*numPatches.y*2; ++y) {
		nv::vec2i startingPos;
		srand(time(NULL));
		startingPos.x = rand() % (_originalTextures[0]->getImage().getWidth() - (int) sizePatches.x);
		startingPos.y = rand() % (_originalTextures[0]->getImage().getHeight() - (int) sizePatches.y);
		_patchListIndexed[y] = new DSIndexedTexture(startingPos, sizePatches, *_originalTextures[_originalTextures.size()-1]);
	}

	// create a random texture to iterate over
	std::cout << "==========CREATING RANDOM TEXTURE========\n";
	*_patchedTexture = createRandomTexture(*_patchedTexture, _patchListIndexed, numPatches);
	_patchedTexture->saveToFile(FIF_JPEG, "../data/textures/random.jpg"); 
	std::cout << "=======DONE=======\n";
		
	_synthesizedTextures.push_back(_patchedTexture);

	nv::vec2i sizeScaledPatched(128,128);

	//optimization
	std::cout << "==========DISCREET OPTIMIZATION========\n";

	for(int s = _originalTextures[0]->getNeighbourSizes().size()-1; s >= 0; --s) {
		std::cout << "s: " << s; 
		std::cout << " size: " << _originalTextures[0]->getNeighbourSize(s) << std::endl;
		*_patchedTexture = discreetOptimization(*_patchedTexture, *_originalTextures[0], iterations, s);
	}

	int numIterations = ceil(iterations/2);
	for(int i = 1; i < _originalTextures.size(); ++i) {
		std::cout << "==discreet optimization (" << i << ") starting\n";
		_patchedTexture->doubleSize(*_originalTextures[i]);
		_patchedTexture->saveToFile(FIF_JPEG, "../data/textures/synth.jpg");
		for(int s = _originalTextures[i]->getNeighbourSizes().size()-1; s >= 0; --s) {
			std::cout << "s: " << s;
			std::cout << " size: " << _originalTextures[i]->getNeighbourSize(s) << std::endl;
			*_patchedTexture = discreetOptimization(*_patchedTexture, *_originalTextures[i], iterations, s);
		}
		std::cout << "==discreet optimization (" << i << ") done\n=\n";
		numIterations = ceil(numIterations/2);
	}

	std::cout << "=======DONE=======\n";
	_patchedTexture->saveToFile(FIF_JPEG, "../data/textures/synthdiscreet.jpg");
}

void DSTextureSynthesis::loadImage(std::string const& path) {
	_originalTextures.clear();
	_imagePath = path;
	_originalImage = new Image(_imagePath.c_str());

	nv::vec2i currentSize(_originalImage->getWidth(), _originalImage->getHeight());

	std::vector<Image> images;
	DSTexture first(_originalImage);
	images.push_back(*_originalImage);

	while(currentSize.x > 40 && currentSize.y > 40 ) {
		currentSize.x = ceil(currentSize.x / 2);
		currentSize.y = ceil(currentSize.y / 2);
		first.halfSize();
		images.push_back(first.getImage());
	}

	// loop through all resolutions and create a k-coherence map
	for(int i = images.size()-1; i >= 0; --i) {
		Image currentImage = images[i];
		DSCoherentTexture* coTex = nullptr;
		if(i == images.size() - 1) 
			coTex = new DSCoherentTexture(new Image(currentImage));
		else
			coTex = new DSCoherentTexture(new Image(currentImage), *_originalTextures.back());
		_originalTextures.push_back(coTex);
	}
}

Image const DSTextureSynthesis::scaleImage(nv::vec2i const& sizeScaled, Image const& image) {
	Image temp = image;
	temp.scale((unsigned int)sizeScaled.x, (unsigned int)sizeScaled.y);
	return temp;
}

// blends between pixels according to the ratio provided
nv::vec3f const DSTextureSynthesis::featheredPixel(nv::vec3f const& patchedPixel, nv::vec3f const& patchPixel, float ratio) {
	float red	= patchedPixel.x * ratio + patchPixel.x * (1-ratio);
	float green = patchedPixel.y * ratio + patchPixel.y * (1-ratio);
	float blue	= patchedPixel.z * ratio + patchPixel.z * (1-ratio);

	return nv::vec3f(red, green, blue);
}

// This is the logic for the discreetOptimization, similar to Texture optimization but using the k-coherence algorithm
DSIndexedTexture const DSTextureSynthesis::discreetOptimization(DSIndexedTexture const& patchedTexture, DSCoherentTexture& sampleTexture, int iterations, int neighbourSizeIndex) {
	//number of iterations
	Image image = patchedTexture.getImage();
	
	DSIndexedTexture oldTexture(patchedTexture);
	int oldDist = 0;

	DSIndexedTexture newTexture(patchedTexture);
	int newDist = 0;

	newTexture.clearPixelValues();
	int neighbourSize = sampleTexture.getNeighbourSize(neighbourSizeIndex);

	//hold current best distance for each pixel
	std::vector<std::vector< std::vector<nv::vec2i*> >> distanceList;
	distanceList.resize(oldTexture.getSize().x);
	for(int x = 0; x < distanceList.size(); ++x)
		distanceList[x].resize(oldTexture.getSize().y);

	std::vector<std::vector< std::vector<nv::vec2i> >> closestNeighbours;
	closestNeighbours.resize(oldTexture.getSize().x);
	for(int x = 0; x < distanceList.size(); ++x)
		closestNeighbours[x].resize(oldTexture.getSize().y);

	nv::vec2i sizeScaledPatched(128,128);
	for(int n = 0; n < iterations; ++n) {
		/*for(int x = 0; x < oldTexture.getSize().x; ++x) {
			for(int y = 0; y < oldTexture.getSize().y; ++y) {
				distanceList[x][y];
			}
		}*/

		newDist = 0;
		newTexture.clearPixelValues();

		// this loops through each pixel in the neighbourhood of the pixel being synthesized
		// atm there is a border of pixels not being synthesized, should implement wrapping or mirroring of pixels
		//int min = 0;
		int xMax = oldTexture.getSize().x;
		int yMax = oldTexture.getSize().y;
		int iterationDistance = (neighbourSize*2+1) / 4;

		int xIter = floor((xMax-1) / iterationDistance);
		int yIter = floor((yMax-1) / iterationDistance);
		
		#pragma omp parallel for shared(closestNeighbours) num_threads(8)
		for(int x = 0; x < xMax; x += iterationDistance) {
			for(int y = 0; y < yMax; y += iterationDistance) {
				nv::vec2i current(x,y);

				DSPixel closest;
				std::priority_queue<DSPixel, std::vector<DSPixel>, DSComparePixels> pixelList = sampleTexture.findClosestNeighbourhood(oldTexture, current, neighbourSizeIndex, _quality);

				for(int p = 0; p < pixelList.size(); ++p) {
					closest = pixelList.top();
					
					nv::vec2i bottomRight(closest.position);
					bottomRight.x += neighbourSize;
					bottomRight.y += neighbourSize;

					nv::vec2i topLeft = closest.position;
					topLeft.x -= neighbourSize;
					topLeft.y -= neighbourSize;

					if(sampleTexture.isInBounds(bottomRight) && sampleTexture.isInBounds(topLeft))
						break;

					pixelList.pop();
				}

				for(int i = -neighbourSize; i < neighbourSize; ++i) {
					for(int j = -neighbourSize; j < neighbourSize; ++j) {
						
						nv::vec2i closestTemp(closest.position);
						closestTemp.x += i;
						closestTemp.y += j;

						nv::vec2i currentTemp = current;
						currentTemp.x += i;
						currentTemp.y += j;

						currentTemp = oldTexture.wrapNotInBounds(currentTemp);
						closestTemp = sampleTexture.mirrorNotInBounds(closestTemp);
						
						#pragma omp critical
						{
							closestNeighbours[currentTemp.x][currentTemp.y].push_back(closestTemp);
						}
					}
				}
			}
		}

		// find closest
		#pragma omp parallel for num_threads(8)
		for(int x = 0; x < xMax; ++x) {
			for(int y = 0; y < yMax; ++y) {
				DSPixel nextClosest;
				
				// offset from the centre of the closest matching neighbour
				// the current position in the neighbourhood (of oldTexture)
				nv::vec2i currentTemp(x,y);
				sampleTexture.findCandidates(oldTexture, currentTemp, neighbourSizeIndex, _quality, distanceList[currentTemp.x][currentTemp.y]);

				nv::vec2i currentPoint;
				int currentDist = 0;
				int lowestDist = 10000000;

				//loop through the candidate vector
				int count = 0;
				for(int t = 0; t < distanceList[currentTemp.x][currentTemp.y].size(); ++t) {
					currentPoint = sampleTexture.mirrorNotInBounds(*distanceList[currentTemp.x][currentTemp.y][t]);

					// get the neighbour distance
					for(int c = 0; c < closestNeighbours[currentTemp.x][currentTemp.y].size(); ++c) {
						nv::vec2i closestTemp = closestNeighbours[currentTemp.x][currentTemp.y][c];
						closestTemp = sampleTexture.mirrorNotInBounds(closestTemp);
						currentDist = sampleTexture.euclideanDistanceSquared(closestTemp, sampleTexture, currentPoint);

						if(currentDist < lowestDist) {
							#pragma omp critical
							{
								lowestDist = currentDist;
								newTexture.setPixel(currentTemp.x, currentTemp.y, *sampleTexture.getPixels()[closestTemp.x][closestTemp.y]);
								newTexture.setPixelLocation(currentTemp.x, currentTemp.y, closestTemp);
							}
						}
						++count;
					}
					currentDist /= count;
					count = 0;
				}
				oldDist = newDist;
				_synthesizedTextures.push_back(new DSIndexedTexture(newTexture));
			}
		}
	}
	return oldTexture;
}

// creates a random texture from patches with each pixel location
DSIndexedTexture const DSTextureSynthesis::createRandomTexture(DSIndexedTexture& patchedTexture, std::vector<DSIndexedTexture*> const& patchList, nv::vec2i const& numPatches) {
	// loop through all patches
	for(int x = 0; x < numPatches.x; ++x) {
		for(int y = 0; y < numPatches.y; ++y) {
			// select random patch
			srand(time(NULL));
			int randomPatch = rand() % (numPatches.x*numPatches.y*2);
			DSIndexedTexture patchDetails = *patchList[randomPatch];

			// place patch in the texture
			for(int i = 0; i < patchDetails.getSize().x; ++i) {
				for(int j = 0; j < patchDetails.getSize().y; ++j) {
					nv::vec2i pixelPosition(x*patchDetails.getSize().x + i, y*patchDetails.getSize().y + j);
					if(patchedTexture.isInBounds(pixelPosition)) {
						patchedTexture.setPixel(pixelPosition.x, pixelPosition.y, *patchDetails.getPixels()[i][j]);
						patchedTexture.setPixelLocation(pixelPosition.x, pixelPosition.y, *patchDetails.getPixelLocations()[i][j]);
					}
				}
			}
		}
	}
	return patchedTexture;
}

DSTexture const DSTextureSynthesis::discreetOptimization(DSTexture const& patchedTexture, DSTexture const& sampleTexture, int iterations) {
	Image image = patchedTexture.getImage();
	
	DSTexture oldTexture(new Image(image));
	DSTexture newTexture(new Image(image));
	newTexture.clearPixelValues();
	
	//width of surrounding pixels - value of 2 corresponds to 5x5 matrix
	int neighbourSize = 2;
	int neighbourIncrement = (int)((neighbourSize*2+1) / 4);

	for(int n = 0; n < iterations; ++n) {
		newTexture.clearPixelValues();
		nv::vec2i closest;
		nv::vec2i current;
		
		for(int x = neighbourSize; x < oldTexture.getSize().x - neighbourSize; x += neighbourIncrement) {
			for(int y = neighbourSize; y < oldTexture.getSize().y - neighbourSize; y += neighbourIncrement) {
				current.x = x;
				current.y = y;
				closest = sampleTexture.findClosestNeighbourhood(oldTexture, current, neighbourSize);
				nv::vec2i posPatch;
				nv::vec2i posSample;
				
				for(int i = -neighbourSize; i <= neighbourSize; ++i) {
					for(int j = -neighbourSize; j <= neighbourSize; ++j) {
						posSample.x = closest.x + i;
						posSample.y = closest.y + j;
						posPatch.x = x+i;
						posPatch.y = y+j;

						if(oldTexture.isInBounds(posPatch) && sampleTexture.isInBounds(posSample)) {
							float red   = newTexture.getPixels()[posPatch.x][posPatch.y]->x;
							float blue  = newTexture.getPixels()[posPatch.x][posPatch.y]->y;
							float green = newTexture.getPixels()[posPatch.x][posPatch.y]->z;
							if(red <= 0.00001 && green <= 0.00001 && blue <= 0.00001) {
								newTexture.setPixel(posPatch.x,
													posPatch.y,
													*sampleTexture.getPixels()[posSample.x][posSample.y]
								);
							}
							else {
								newTexture.setPixel(posPatch.x,
													posPatch.y,
													featheredPixel( *newTexture.getPixels()[posPatch.x][posPatch.y],
																	*sampleTexture.getPixels()[posSample.x][posSample.y],
																	0.5f)
								); 
							}
						}
					}
				}
			}
		}
		oldTexture = DSTexture(newTexture);
	}
	return oldTexture;
}

// creates a random texture from patches
DSTexture const DSTextureSynthesis::createRandomTexture(DSTexture& patchedTexture, std::vector<DSTexture*> const& patchList, nv::vec2i const& numPatches, int overlap) {
	// loop through all patches
	for(int x = 0; x < numPatches.x; ++x) {
		for(int y = 0; y < numPatches.y; ++y) {
			//select random patch to place
			srand(time(NULL));
			int randomPatch = rand() % (numPatches.x * numPatches.y * 2);
			DSTexture patchDetails(*patchList[randomPatch]);

			//place patch in the texture
			for(int i = 0; i < patchDetails.getSize().x; ++i) {
				for(int j = 0; j < patchDetails.getSize().y; ++j) {
					nv::vec2i pixelPosition(x*patchDetails.getSize().x + i, y*patchDetails.getSize().y + j);
					if(patchedTexture.isInBounds(pixelPosition))
						patchedTexture.setPixel(pixelPosition.x, pixelPosition.y, *patchDetails.getPixels()[i][j]);
				}
			}
		}
	}
	return patchedTexture;
}
