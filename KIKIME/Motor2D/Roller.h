#pragma once
#include "Entity.h"

class Roller : public Entity
{
private:

public:
	Roller(int x, int y);
	~Roller();

	void Move();
	void Shoot();
};