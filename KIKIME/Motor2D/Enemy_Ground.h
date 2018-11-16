#pragma once
#include "Enemy.h"

class Enemy_Ground : public Enemy 
{
private:

public:
	Enemy_Ground(int x, int y);

	void Move();
	void Shoot();
};
