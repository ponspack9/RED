#include "vec3.h"


int main() {

	vec3<int> v1(1, 2, 3);
	vec3<int> v2(v1);
	vec3<int> v3;
	
	cout << v1.operator+=(3) << endl;

	system("pause");
	return 0;
}