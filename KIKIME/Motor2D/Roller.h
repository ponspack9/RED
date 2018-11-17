#pragma once
#include "Entity.h"
#include "p2DynArray.h"

class Roller : public Entity
{
private:

	p2DynArray<iPoint>* path;

public:

	Roller() {}
	Roller(iPoint pos, Entity*e);
	~Roller();

	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Shoot();

	bool UpdateLogic();

public: 
};