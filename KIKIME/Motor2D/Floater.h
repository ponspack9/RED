#pragma once
#include "Entity.h"

class Floater : public Entity 
{
private:

public:
	Floater(int x,int y);
	~Floater();

	void Move();
	void Shoot();
};
