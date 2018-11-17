#pragma once
#include "Entity.h"
#include "p2DynArray.h"

class Floater : public Entity 
{
private:

	p2DynArray<iPoint>* path;

public:

	Floater() {}
	Floater(iPoint pos, Entity* e,entityType type);

	~Floater();

	bool Update(float dt);
	bool PostUpdate();

	bool UpdateLogic(iPoint pos);
};
