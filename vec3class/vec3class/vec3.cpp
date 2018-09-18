#include "vec3.h"


//CONSTRUCTORS DEFINITION


template<class type>
vec3<type>::vec3():
	x(0),
	y(0),
	z(0)
{}

template<class type>
vec3<type>::vec3(type x_, type y_, type z_):		
		x(x_),
		y(y_),
		z(z_)
{}		  

template<class type>
vec3<type>::vec3(const vec3 &vec):
	x(vec.x),
	y(vec.y),
	z(vec.z)
{}


//METHODS DEFINITION


template<class type>
bool vec3<type>::is_zero()
{
	if (this->x == 0 && this->y == 0 && this->z == 0) {
		return true;
	}
	else {
		return false;
	}
}

template<class type>
void vec3<type>::normalize()
{
	type aux = module();
	if (aux != 0) {
		x = x / aux; 
		y = y / aux;
		z = z / aux;
	}
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
	return type(sqrt((vec.x-x)*(vec.x - x)
					+(vec.y-y)*(vec.y - y)
					+(vec.z-z)*(vec.z - z)));
}

template<class type>
void vec3<type>::printScreen()
{
	cout << this->x << " , " << this->y << " , " << this->z << endl;
}

template<class type>
type vec3<type>::module()
{
	return type(sqrt(x*x + y*y + z*z));
}


//OPERATORS DEFINITION


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
	if (this->x == vec.x && this->y == vec.y && this->z == vec.z){
	return true;
	}
	else {
		return false;
	}
}



int main() {

	cout << "TAMO READY?" << endl;
	cout << "AGARRARSUS" << endl;

	vec3<float> v1;
	vec3<float> v2(1, 2, 3);
	vec3<float> v3(v2);
	
	cout << "v1:";
	v1.printScreen();
	cout << "v2:";
	v2.printScreen();
	cout << "v3:";
	v3.printScreen();
	cout << endl;

	bool aux1 = v1.is_zero();
	bool aux2 = v2.is_zero();
	bool aux3 = v3.is_zero();

	(aux1 == true) ? cout << "V1 is zero" << endl : cout << "V1 no nene" << endl;
	(aux2 == true) ? cout << "V2 is zero" << endl : cout << "V2 no nene" << endl;
	(aux3 == true) ? cout << "V3 is zero" << endl : cout << "V3 no nene" << endl;
	cout << endl;

	v1.operator+=(3);
	v3.operator-=(v1);
	v2.normalize();

	cout << "New v1:";
	v1.printScreen();
	cout << "New v2:";
	v2.printScreen();
	cout << "New v3:";
	v3.printScreen();
	cout << endl;

	v2.zero();
	float D = v1.distance_to(v3);
	aux2 = v2.is_zero();

	cout << "Distance between v1:";
	v1.printScreen();
	cout << "and v3:";
	v3.printScreen();
	cout << "is:\t" << D << endl;
	cout << "New v2:";
	v2.printScreen();
	(aux2 == true) ? cout << "V2 is zero" << endl : cout << "V2 no nene" << endl;
	cout << endl;

	v1.zero();
	aux1 = v1.operator==(v2);
	aux2 = v2.operator==(v3);

	cout << "New v1:";
	v1.printScreen();
	cout << "New v2:";
	v2.printScreen();
	cout << "New v3:";
	v3.printScreen();
	cout << endl;

	(aux1 == true) ? cout << "v1 & v2 igualiko moreno" << endl : cout << "v1 & v2 ni de flys" << endl;
	(aux2 == true) ? cout << "v2 & v3 igualiko moreno" << endl : cout << "v2 & v3 ni de flys" << endl;

	system("pause");
	return 0;
}

