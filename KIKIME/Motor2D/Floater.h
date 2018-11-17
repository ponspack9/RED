#pragma once
#include "Entity.h"

class Floater : public Entity 
{
private:

public:

	Floater() {}
	Floater(iPoint pos, Entity* e, SDL_Texture* sprites);
	~Floater();

	bool Update(float dt);
	bool PostUpdate();

	void Draw();
	void Move();
	void Shoot();

	bool UpdateLogic(iPoint pos);

public:
	SDL_Texture * sprites = nullptr;
};
