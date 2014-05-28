#ifndef MATHLIB_HPP
#define MATHLIB_HPP

#include "sdk_swak.h"

namespace Mathlib {
	float distance(nv::vec4f const&, nv::vec4f const&);
	float angle(nv::vec4f const&, nv::vec4f const&);
	nv::vec3<float> cross(nv::vec3<float> const&, nv::vec3<float> const&);
}

#endif