#pragma once

#include "p2Point.h"
#include "j1Module.h"
#include "SDL\include\SDL_timer.h"
#include "p2SString.h"
#include "p2List.h"
#include "j1App.h"
#include "Entity.h"
#include "Floater.h"
#include "Roller.h"



//class Entity;
struct SDL_Texture;
//struct Collider;

class j1EntityManager : public j1Module 
{

private:

	float	current_time = 0.0f;
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
	bool Save(pugi::xml_node &node);
	bool Load(pugi::xml_node &node);

	void UpdateAll(float dt, bool run);

	//Creators

	Entity*	 CreateEntity(entityType type, iPoint pos);
	bool	 DeleteEntity(Entity* entity);	

public:

	
	SDL_Texture*	enemyTex = nullptr;
	SDL_Texture*	playerTex = nullptr;
	p2SString		enemyPath;
	p2SString		playerPath;
	p2List<Entity*> entities;	

	Floater		    floaterinfo;
	Roller			rollerinfo;
	//Entity*			playerinfo = new Entity();
};

