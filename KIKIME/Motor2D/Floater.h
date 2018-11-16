#pragma once
#include "Entity.h"

class Floater : public Entity 
{
private:

public:
	Floater(iPoint pos);
	~Floater();

	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node & node, const p2List<Entity*>* entities) const;
	bool Load(pugi::xml_node & node, p2List<Entity*>* entities);

	void Move();
	void Shoot();
};
