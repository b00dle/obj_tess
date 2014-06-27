#ifndef __PATH__H__
#define __PATH__H__

#include "sdk_swak.h"

#include <vector>
#include <iostream>


struct Pathpoint
{
	Pathpoint(nv::matrix4f point , int sample)
	{
		mValue = point;
		mIsSamplePoint = sample;
	}
	nv::matrix4f	mValue;
	int			mIsSamplePoint;

};



class Path
{
	public:

		Path();
		Path(nv::matrix4f const&);
		Path(std::vector<nv::matrix4f> const&);

		virtual void                addSegment(nv::matrix4f const&);

		float                       length();
		float                       length(int , int);

		//Getter
		std::vector<Pathpoint>      Points();
		Pathpoint                   Point(int);
		nv::vec3f                   getTangent(int);
		nv::vec3f                   getNormal(int);
		std::vector<nv::vec3f>*     vertices();
		std::vector<unsigned int>*  indices();
		std::vector<nv::matrix4f>      getStitchPoints();
		int samplePointSize();

		//Path calculation
		virtual void                calculate();

	protected:
		
		std::vector<nv::matrix4f>      mSampleMatrix;
		std::vector<nv::vec4f>      mControlPoints;

		std::vector<Pathpoint>      mPointsOnPath;
		std::vector<nv::vec3f>      mTangents;
		std::vector<nv::vec3f>      mNormals;

		
		std::vector<nv::vec3f>      mVerticesPath;
		std::vector<unsigned int>   mIndicesPath;

		float                       mScaleFactor;
		float                       mLength;
};

#endif
