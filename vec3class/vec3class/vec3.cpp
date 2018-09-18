#include "vec3.h"

using namespace std;

template<class type>
vec3<type>::vec3()
{
	x = 0;
	y = 0;
	z = 0;
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
	if (this->x = 0 && this->y = 0 && this->z = 0) {
		return true;
	}
	else {
		return false;
	}
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

template<class type>
void vec3<type>::printScreen()
{
	cout << this->x << " , " << this->y << " , " << this->z << endl;
}

template<typename type>
vec3<type> vec3<type>::operator+(const vec3 & vec)
{	
	return vec3(x + vec.x,
				z + vec.z,
				y + vec.y);
}

template<class type>
vec3<type> vec3<type>::operator+(type num)
{
	return vec3(x + num,
				y + num,
				z + num);
}

template<typename type>
vec3<type> vec3<type>::operator-(const vec3 & vec)
{
	return vec3(x - vec.x,
				z - vec.z,
				y - vec.y);
}

template<class type>
vec3<type> vec3<type>::operator-(type num)
{
	return vec3(x - num,
				y - num,
				z - num);
}

template<typename type>
void vec3<type>::operator+=(const vec3 &vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
}

template<class type>
void vec3<type>::operator+=(type num)
{
	x += num;
	y += num;
	z += num;
}

template<typename type>
void vec3<type>::operator-=(const vec3 &vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
}

template<class type>
void vec3<type>::operator-=(type num)
{
	x -= num;
	y -= num;
	z -= num;
}

template<class type>
vec3<type> vec3<type>::operator=(const vec3 & vec)
{
	return vec3(x = vec.x, y = vec.y, z = vec.z);
}

template<class type>
vec3<type> vec3<type>::operator=(type num)
{
	return vec3(x = num, y = num, z = num);
}


template<typename type>
bool vec3<type>::operator==(const vec3 & vec)
{
	if (this->x = vec.x && this->y = vec.y && this->z = vec.z){
	return true;
	}
	else {
		return false;
	}
}

