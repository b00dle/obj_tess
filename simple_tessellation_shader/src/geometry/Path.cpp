#include "Path.h"
#include "Mathlib.h"


//C'tor
Path::Path():mScaleFactor(0.4f)
{}

Path::Path(nv::matrix4f const& coSysteme):mScaleFactor(0.4f)
{
	mSampleMatrix.push_back(coSysteme);
}
Path::Path(std::vector<nv::matrix4f> const& matrix ): mScaleFactor(0.4f), mSampleMatrix(matrix)
{}


//generate Path
void Path::addSegment(nv::matrix4f const& coSysteme)
{
	mSampleMatrix.push_back(coSysteme);
}

//Path properties
float Path::length()
{
	return mLength;	
}

void Path::calculate()
{
	//do nothing
	for(int i = 0 ; i < mSampleMatrix.size(); ++i)
	{
		mPointsOnPath.push_back(Pathpoint(mSampleMatrix[i],i));
		mVerticesPath.push_back(nv::vec3f(mSampleMatrix[i].get_column(3)));
	}
	for(int i = 0; i < mSampleMatrix.size()-1; ++i)
	{
		mIndicesPath.push_back(i);
		mIndicesPath.push_back(i+1);
	}
	
}

float Path::length(int i , int j)
{
	float toReturn= 0.0f;
	
	if(i < j)
	{
		for(int k = i ; k <j-1 ; ++k)
		{
			toReturn += Mathlib::distance(mPointsOnPath[k].mValue.get_column(3), mPointsOnPath[k+1].mValue.get_column(3));
		}
		return toReturn;
	}
	else
	{
		for(int k = j ; k <i-1 ; ++k)
		{
			toReturn += Mathlib::distance(mPointsOnPath[k].mValue.get_column(3), mPointsOnPath[k+1].mValue.get_column(3));
		}
		return toReturn;
	}

}

//Getter

std::vector<nv::vec3f>* Path::vertices()
{
	return &mVerticesPath;
}


std::vector<unsigned int>* Path::indices()
{
	return &mIndicesPath;
}


std::vector<Pathpoint> Path::Points()
{
	return mPointsOnPath;
}

Pathpoint Path::Point(int i)
{
	return mPointsOnPath[i];
}


nv::vec3f Path::getTangent(int i)
{
	return mTangents[i];
}


nv::vec3f Path::getNormal(int i)
{
	return mNormals[i];
}

std::vector<nv::matrix4f> Path::getStitchPoints()
{
	if(mSampleMatrix.size() <=2)
	{
		return mSampleMatrix;
	}
	else
	{
		return std::vector<nv::matrix4f>(mSampleMatrix.end()-3,mSampleMatrix.end());
	}
	
}

int Path::samplePointSize()
{
	return mSampleMatrix.size();
}

