#pragma once
#include "Entity.h"

class Static : public Entity
{
private:

public:
	Static(iPoint pos, Entity* e);
	~Static();

	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node & node, const p2List<Entity*>* entities) const;
	bool Load(pugi::xml_node & node,  p2List<Entity*>* entities);

	void Move();
	void Shoot();
};