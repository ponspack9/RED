#ifndef VEC3_H_
#define VEC3_H_

#include<cstdio>
#include<cstdlib>
#include <iostream>

using namespace std;

template<class type> class vec3 {

private:

	type x, y, z;

public:

	/*CONSTRUCTORS*/
	vec3();

	vec3(type x_, type y_, type z_);

	vec3(const vec3 &vec);

	/*DESTRUCTOR*/
	~vec3() {}

public://METHODS

	bool is_zero(const vec3 &vec);
	void normalize();
	void zero();
	type distance_to(const vec3 &vec);


public://OPERATORS




};

#endif // !VEC3_H_


