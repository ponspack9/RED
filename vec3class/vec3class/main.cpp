#include "vec3.h"



int main(){

	cout << "TEST" << endl;

	vec3<int> v1;
	vec3<int> v2(5, 2, -2);
	vec3<int> v3(v2);

	cout << "v1:";
	v1.printScreen();
	cout << "v2:";
	v2.printScreen();
	cout << "v3:";
	v3.printScreen();
	cout << endl;

	system("pause");
	return 0;
}