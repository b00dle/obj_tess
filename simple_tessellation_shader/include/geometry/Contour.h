#ifndef __CONTOUR__H__
#define __CONTOUR__H__

#include "sdk_swak.h"

#include <vector>


class Contour
{
	public:
		Contour();
		Contour(std::vector<nv::vec4f>);
		

		nv::vec4f                bezierPoint( float ,  nv::vec4f const& , nv::vec4f const&, nv::vec4f const& , nv::vec4f const& );

		void                     calculateVertices();
		void                     calculateContourRecursive(float, float, nv::vec4f const& , nv::vec4f const& , nv::vec4f const& , nv::vec4f const&); 
		void                     calculateControlPoints();
		
		std::vector<nv::vec3f>*  vertices();
		int                      numberOfPoints();
		nv::vec4f                Bezierpoint(int);

	private:
		std::vector<nv::vec4f>   mControlPoints;
		std::vector<nv::vec4f>   mBezierPoints;
		std::vector<nv::vec4f>   mEndPoints;
		std::vector<nv::vec3f>   mVertices;
};

#endif