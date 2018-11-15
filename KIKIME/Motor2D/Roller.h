#pragma once
#include "Entity.h"

class Roller : public Entity
{
private:

public:
	Roller(int x, int y);
	~Roller();

	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node & node) const;
	bool Load(pugi::xml_node & node);

	void Move();
	void Shoot();
};