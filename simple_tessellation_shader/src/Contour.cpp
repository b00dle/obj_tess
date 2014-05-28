#include"Contour.h"
#include "Mathlib.h"

Contour::Contour(): mControlPoints()
{
	std::vector<nv::vec4f> temp;
	temp.push_back(nv::vec4f(1,0,1,1));
	temp.push_back(nv::vec4f(-1,0,1,1));
	temp.push_back(nv::vec4f(-1,0,-1,1));
	temp.push_back(nv::vec4f(1,0,-1,1));
	temp.push_back(nv::vec4f(1,0,1,1));

	mEndPoints= std::vector<nv::vec4f>(temp);
	calculateControlPoints();
	calculateVertices();
}

Contour::Contour(std::vector<nv::vec4f> initList): mEndPoints(initList),mVertices() , mControlPoints()
{
	mEndPoints.push_back(mEndPoints[0]);
	calculateControlPoints();
	calculateVertices();
}

 
std::vector<nv::vec3f>* Contour::vertices()
{
	return &mVertices;
}

nv::vec4f Contour::bezierPoint(float t ,nv::vec4f const& start , nv::vec4f const& control1 , nv::vec4f const& control2 , nv::vec4f const& end)
{
	return (pow((1.0f-t),3.0f)*start + 3* pow((1.0f - t),2.0f)*t*control1  + 3* (1.0f - t)*pow(t,2.0f)*control2 + pow(t,3.0f)*end); 
}

void Contour::calculateContourRecursive(float min , float max , nv::vec4f const& start , nv::vec4f const& control1 , nv::vec4f const& control2 , nv::vec4f const& end)
{
	float middle = (min + max) / 2.0f;
	nv::vec4f pointBetween = bezierPoint(middle,start , control1 , control2 , end); 

	

	if( abs(middle-max) > 0.1f && abs(middle -min) > 0.1f && (Mathlib::angle(nv::normalize(pointBetween-start), nv::normalize(pointBetween-end)) > 120.0f))
	{
		calculateContourRecursive(min,middle,start, control1, control2, end);
		calculateContourRecursive(middle,max,start, control1, control2, end);
	}
	else
	{
		mVertices.push_back(nv::vec3f(pointBetween));
		mBezierPoints.push_back(pointBetween);
	}

}

void  Contour::calculateVertices()
{
	mVertices.clear();
	mBezierPoints.clear();
	
	nv::vec4f start,control1,control2,end;
		
	for(int i = 1; i <= mControlPoints.size() - 3 ; i+=3)
	{
		////add start
		mVertices.push_back(nv::vec3f(mControlPoints[i]));
		
		mBezierPoints.push_back(mControlPoints[i]);

		start = mControlPoints[i%mControlPoints.size()];
		control1 = mControlPoints[(i+1)%mControlPoints.size()];
		control2 = mControlPoints[(i+2)%mControlPoints.size()];
		end = mControlPoints[(i+3)%mControlPoints.size()];

		for(int k = 1; k < 6; ++k)
			{
				float t = float(k)/6.0f;
				nv::vec4f pointBetween = bezierPoint(t,start,control1,control2,end);
				mVertices.push_back(nv::vec3f(pointBetween));
				mBezierPoints.push_back(pointBetween);
			}
	}	
}

void Contour::calculateControlPoints()
{
	mControlPoints.clear();

	for(int i = 0; i < mEndPoints.size() ; i++)
	{
		if ( i == 0)
		{
			nv::vec4f p0 = mEndPoints[mEndPoints.size()-2];
			nv::vec4f p1 = mEndPoints[i];
			nv::vec4f p2 = mEndPoints[i+1];

			nv::vec4f tangent = nv::normalize(p2-p0);
			nv::vec4f q0 = p1 - (0.3f*tangent) * nv::length(p1-p0) ;
			nv::vec4f q1 = p1 + (0.3f*tangent) * nv::length(p2-p1);

			mControlPoints.push_back(q0);
			mControlPoints.push_back(p1);
			mControlPoints.push_back(q1);
		}
		else if ( i == mEndPoints.size()-1)
		{
			nv::vec4f p0 = mEndPoints[i-1];
			nv::vec4f p1 = mEndPoints[i];
			nv::vec4f p2 = mEndPoints[1];

			nv::vec4f tangent = nv::normalize(p2-p0);
			nv::vec4f q0 = p1 - (0.3f*tangent) * nv::length(p1-p0) ;
			nv::vec4f q1 = p1 + (0.3f*tangent) * nv::length(p2-p1);

			mControlPoints.push_back(q0);
			mControlPoints.push_back(p1);
			mControlPoints.push_back(q1);
		}
		else
		{
			nv::vec4f p0 = mEndPoints[i-1];
			nv::vec4f p1 = mEndPoints[i];
			nv::vec4f p2 = mEndPoints[i+1];

			nv::vec4f tangent = nv::normalize(p2-p0);
			nv::vec4f q0 = p1 - (0.3f*tangent) * nv::length(p1-p0) ;
			nv::vec4f q1 = p1 + (0.3f*tangent) * nv::length(p2-p1);

			mControlPoints.push_back(q0);
			mControlPoints.push_back(p1);
			mControlPoints.push_back(q1);
		}
	}
}

int Contour::numberOfPoints()
{
	return mVertices.size();
}

nv::vec4f Contour::Bezierpoint(int i)
{
	return mBezierPoints[i];
}