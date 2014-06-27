#include "Bezierpath.h"
#include "Mathlib.h"

#include <iostream>
 
//C'tor
BezierPath::BezierPath()
{}
BezierPath::BezierPath(nv::matrix4f const& coSys)
{
	mSampleMatrix.clear();
	mSampleMatrix.push_back(coSys);
}
BezierPath::BezierPath(std::vector<nv::matrix4f> const& list )
{
	mSampleMatrix = list;
}


//Private path calculations
nv::vec4f BezierPath::bezierPoint(float t ,nv::vec4f const& start , nv::vec4f const& control1 , nv::vec4f const& control2 , nv::vec4f const& end)
{
	return (pow((1.0f-t),3.0f)*start + 3* pow((1.0f - t),2.0f)*t*control1  + 3* (1.0f - t)*pow(t,2.0f)*control2 + pow(t,3.0f)*end); 
}


//Path calculation
void BezierPath::calculatePathRecursive(float min , float max , nv::vec4f const& start , nv::vec4f const& control1 , nv::vec4f const& control2 , nv::vec4f const& end, int position)
{
	float middle = (min + max) / 2.0f;
	nv::vec4f pointBetween = bezierPoint(middle,start , control1 , control2 , end); 
	std::cout << 180.0f - Mathlib::angle(nv::normalize(pointBetween-start), nv::normalize(pointBetween-end)) << std::endl;
	if( abs(middle-max) > 0.001f && abs(middle -min) > 0.001f && ((180.0f - Mathlib::angle(nv::normalize(pointBetween-start), nv::normalize(pointBetween-end))) > 10.0f))
	{
		calculatePathRecursive(min,middle,start, control1, control2, end, position);
		calculatePathRecursive(middle,max,start, control1, control2, end, position);
	}
	else
	{
		
		nv::vec4f first		= nv::normalize( mSampleMatrix[position].get_column(0)*(1.0f - middle) + mSampleMatrix[position+1].get_column(0)*(middle));
		nv::vec4f second	= nv::normalize( mSampleMatrix[position].get_column(1)*(1.0f - middle) + mSampleMatrix[position+1].get_column(1)*(middle));
		nv::vec4f third		= nv::normalize( mSampleMatrix[position].get_column(2)*(1.0f - middle) + mSampleMatrix[position+1].get_column(2)*(middle));
				
		nv::matrix4f toPush(1.0f);
		toPush.set_column(0, first);
		toPush.set_column(1, second);
		toPush.set_column(2, third);
		toPush.set_column(3, pointBetween);

		mPointsOnPath.push_back(Pathpoint(toPush , -1));

		mVerticesPath.push_back(nv::vec3f(toPush.get_column(3)));
	}

}

void BezierPath::calculatePath()
{
	mVerticesPath.clear();
	mIndicesPath.clear();
	mPointsOnPath.clear();
	
	nv::vec4f start,control1,control2,end;
	int k = 0;
	
		for(int i = 0, v = 0 ; i < mControlPoints.size() - 3 ; i+=3, ++v)
		{
			
			start = mControlPoints[i];
			control1 = mControlPoints[i+1];
			control2 = mControlPoints[i+2];
			end = mControlPoints[i+3];
			
			//add start
			mVerticesPath.push_back(nv::vec3f(start));
			mPointsOnPath.push_back(Pathpoint(mSampleMatrix[v], v));

			for(int k = 1; k < 1; ++k)
			{
				float t = float(k)/4.0f;

				nv::vec4f pointBetween	= bezierPoint(t,start,control1,control2,end);
				nv::vec4f first			= nv::normalize(mSampleMatrix[v].get_column(0)*(1.0f - t) + mSampleMatrix[v+1].get_column(0)*(t));
				nv::vec4f second		= nv::normalize(mSampleMatrix[v].get_column(1)*(1.0f - t) + mSampleMatrix[v+1].get_column(1)*(t));
				nv::vec4f third			= nv::normalize(mSampleMatrix[v].get_column(2)*(1.0f - t) + mSampleMatrix[v+1].get_column(2)*(t));
				
				nv::matrix4f toPush(1.0f);
				toPush.set_column(0, first);
				toPush.set_column(1, second);
				toPush.set_column(2, third);
				toPush.set_column(3, pointBetween);

				mPointsOnPath.push_back(Pathpoint(toPush , -1));

				mVerticesPath.push_back(nv::vec3f(toPush.get_column(3)));
			}
		}

		//add end
		mVerticesPath.push_back(nv::vec3f(mControlPoints.back()));
		mPointsOnPath.push_back(Pathpoint(mSampleMatrix.back(), mSampleMatrix.size()-1));

		mLength = 0.0f;
		for(int i = 0; i < mPointsOnPath.size()-1; ++i)
		{
			mLength+= Mathlib::distance(mPointsOnPath[i].mValue.get_column(3) , mPointsOnPath[i+1].mValue.get_column(3));
		}

}

void BezierPath::calculateControlPoints()
{
	mControlPoints.clear();

	for(int i = 0; i < mSampleMatrix.size() ; i++)
	{
		if ( i == 0)
		{
			nv::vec4f p1 = mSampleMatrix[0].get_column(3);
			nv::vec4f p2 = mSampleMatrix[1].get_column(3);
						
			nv::vec4f tangent =  nv::normalize(p2-p1);
			
			nv::vec4f q1 = p1 + (tangent * mScaleFactor);

			mControlPoints.push_back(p1);
			mControlPoints.push_back(q1);

		}
		else if ( i == mSampleMatrix.size()-1)
		{
			nv::vec4f p0 = mSampleMatrix[i-1].get_column(3);
			nv::vec4f p1 = mSampleMatrix[i].get_column(3);

			nv::vec4f tangent =  nv::normalize(p1-p0);
			nv::vec4f q0 = p1 - (tangent * mScaleFactor);

			mControlPoints.push_back(q0);
			mControlPoints.push_back(p1);

		}
		else
		{

			nv::vec4f p0 = mSampleMatrix[i-1].get_column(3);
			nv::vec4f p1 = mSampleMatrix[i].get_column(3);
			nv::vec4f p2 = mSampleMatrix[i+1].get_column(3);

			nv::vec4f tangent = nv::normalize(p2-p0);
			nv::vec4f q0 = p1 - (tangent * mScaleFactor ) * nv::length(p1-p0);
			nv::vec4f q1 = p1 + (tangent * mScaleFactor ) * nv::length(p2-p1);

			mControlPoints.push_back(q0);
			mControlPoints.push_back(p1);
			mControlPoints.push_back(q1);

		}
	}
}


void BezierPath::calculate()
{
	calculateControlPoints();
	calculatePath();
}
