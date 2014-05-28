#ifndef __BEZIER__PATH__
#define __BEZIER__PATH__

#include "Path.h"

#include "sdk_swak.h"

#include <vector>


class BezierPath : public Path
{
	public:

		//C'tor
		BezierPath();
		BezierPath(nv::matrix4f const&);
		BezierPath(std::vector<nv::matrix4f> const&); 

		//Path calculation
		void calculate();
		
		
	private:
		//Private path calculations
		nv::vec4f bezierPoint(float , nv::vec4f const& , nv::vec4f const& , nv::vec4f const& , nv::vec4f const& );
	
		//Path calculation
		void      calculatePathRecursive(float, float, nv::vec4f const& , nv::vec4f const& , nv::vec4f const& , nv::vec4f const&, int); 
		void      calculateControlPoints();
		void      calculatePath();
	
};

#endif
