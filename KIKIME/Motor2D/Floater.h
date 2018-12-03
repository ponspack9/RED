#pragma once
#include "Enemy.h"

class Floater : public Enemy 
{

public:

	Floater() {}
	Floater(iPoint pos, Enemy* e) : Enemy(pos, e) {}

	~Floater() {}

	bool UpdateLogic(iPoint pos);
};
