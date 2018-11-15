#pragma once
#include "Entity.h"

class Static : public Entity
{
private:

public:
	Static(int x, int y);
	~Static();

	void Move();
	void Shoot();
};