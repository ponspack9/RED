#pragma once
#include "Enemy.h"

class Enemy_Air : public Enemy 
{
private:

public:
	Enemy_Air(int x, int y);

	void Move();
	void Shoot();
};
