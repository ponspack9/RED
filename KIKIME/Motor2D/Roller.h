#pragma once
#include "Enemy.h"

class Roller : public Enemy
{

public:

	Roller() {}
	Roller(iPoint pos, Enemy* e) : Enemy(pos, e) {}

	~Roller() {}

	bool UpdateLogic(iPoint pos);

};