#ifndef VEC3_H_
#define VEC3_H_

#include<cstdio>
#include<cstdlib>

using namespace std;

template<class type>

class vec3 {

private:

	type x, y, z;

public:

	/*CONSTRUCTORS*/
	vec3() : x(0), y(0), z(0) {}

	vec3(type x_, type y_, type z_) : x(x_), y(y_), z(z_) {}

	vec3(const vec3 &vec) : x(vec.x), y(vec.y), z(vec.z) {}

	/*DESTRUCTOR*/
	~vec3() {}

public://METHODS



public://OPERATORS


};

#endif // !VEC3_H_

