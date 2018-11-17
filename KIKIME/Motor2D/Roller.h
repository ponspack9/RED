#pragma once
#include "Entity.h"

class Roller : public Entity
{
private:

public:

	Roller() {}
	Roller(iPoint pos, Entity*e, SDL_Texture* sprites);
	~Roller();

	bool Update(float dt);
	bool PostUpdate();

	void Draw();
	void Move();
	void Shoot();

	bool UpdateLogic();

public: 
	SDL_Texture * sprites = nullptr;
};