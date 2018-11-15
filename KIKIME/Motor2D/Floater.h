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
	bool Save(pugi::xml_node & node) const;
	bool Load(pugi::xml_node & node);

	void Move();
	void Shoot();
};
