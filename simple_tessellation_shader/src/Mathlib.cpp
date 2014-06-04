#include "Mathlib.h"

float Mathlib::distance(nv::vec4f const& lhs, nv::vec4f const& rhs) {
	nv::vec3f tempL(lhs);
	nv::vec3f tempR(rhs);
	return sqrtf(pow(tempR.x-tempL.x, 2) + pow(tempR.y-tempL.y, 2) + pow(tempR.z-tempL.z, 2));;
}

float Mathlib::angle(nv::vec4f const& lhs, nv::vec4f const& rhs) {
	nv::vec3f tempL(lhs);
	nv::vec3f tempR(rhs);
	return acosf( nv::dot(tempL,tempR)/(nv::length(tempL) * nv::length(tempR)) ) * 180/3.14159;
}

nv::vec3<float> Mathlib::cross(nv::vec3<float> const& lhs, nv::vec3<float> const& rhs) { 
	nv::vec3<float> res(0.0f,0.0f,0.0f);
	res[0] = lhs[1]*rhs[2]-lhs[2]*rhs[1];
	res[1] = lhs[2]*rhs[0]-lhs[0]*rhs[2];
	res[2] = lhs[0]*rhs[1]-lhs[1]*rhs[0];
	return res;
}