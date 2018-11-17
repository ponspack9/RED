#pragma once
#include "Entity.h"
#include "p2DynArray.h"

class Roller : public Entity
{
private:

	p2DynArray<iPoint>* path;

public:

	Roller() {}
	Roller(iPoint pos, Entity*e,entityType type);

	~Roller();

	bool Update(float dt);
	bool PostUpdate();

	bool UpdateLogic();

};