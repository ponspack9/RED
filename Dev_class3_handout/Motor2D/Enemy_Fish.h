#pragma once
#include "Enemy.h"

class Enemy_Fish : public Enemy 
{
private:

	iPoint original_position;
	Animation idle;
	Path path;


public:
	Enemy_Fish(int x, int y);
	Collider* collider;

	void Move();
	void Shoot();
};
