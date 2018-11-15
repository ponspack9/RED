#pragma once

#include "p2Point.h"
#include "j1Module.h"
#include "SDL\include\SDL_timer.h"


struct SDL_Texture;
struct Collider;
class Entity;

class j1EntityManager : public j1Module 
{

private:

	float	current_time;
	float	update_cycle;
	bool	do_update;

public:
	j1EntityManager();
	~j1EntityManager(); //virtual

	//Controllers
	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node &node) const;
	bool Load(pugi::xml_node &node);

	void UpdateAll(float dt, bool run);

	//Creators

	Entity*	 CreateEntity(Entity::entityType type, iPoint pos);
	bool	 DeleteEntity(Entity* entity);	

public:

	
	SDL_Texture*	enemyTex = nullptr;
	SDL_Texture*	playerTex = nullptr;
	p2SString		enemyPath;
	p2SString		playerPath;
	p2List<Entity*> entities;	

	Entity*			floaterinfo;
	Entity*			rollerinfo;
	Entity*			playerinfo;
};

