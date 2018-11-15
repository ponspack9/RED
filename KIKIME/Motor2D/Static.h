#pragma once
#include "Entity.h"

class Static : public Entity
{
private:

public:
	Static(int x, int y);
	~Static();

	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node & node) const;
	bool Load(pugi::xml_node & node);

	void Move();
	void Shoot();
};