#pragma once
#include "Enemy.h"

class Enemy_Fish : public Enemy 
{
private:

	iPoint original_position;
	Animation idle;
	//Path path;


public:
	Enemy_Fish(int x, int y);

	void Move();
	void Shoot();
};
