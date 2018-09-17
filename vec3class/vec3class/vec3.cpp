#include "vec3.h"
#include <iostream>
#include <cmath>

using namespace std;

template<class type>
vec3<type>::vec3()
{

}

template<class type>
vec3<type>::vec3(type x_, type y_, type z_)
{
	x = x_;
	y = y_;
	z = z_;
}

template<class type>
vec3<type>::vec3(const vec3 &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

template<class type>
bool vec3<type>::is_zero()
{
	(this->x = 0 && this->y = 0 && this->z = 0) ? return true : return false;
}

template<class type>
void vec3<type>::normalize()
{
	type module = sqrt(this->x ^ 2 + this->y ^ 2 + this->z ^ 2);
	this->x = x / module;
	this->y = y / module;
	this->z = z / module;
}

template<class type>
void vec3<type>::zero()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

template<class type>
type vec3<type>::distance_to(const vec3 &vec)
{
	return type(sqrt((vec.x-this->x)^2
					+(vec.y-this->y)^2
					+(vec.z-this->z)^2));
}

