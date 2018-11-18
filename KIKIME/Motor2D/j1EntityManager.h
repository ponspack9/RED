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
#include "Player.h"



//class Entity;
struct SDL_Texture;
//struct Collider;

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
	void SaveInitialState();
	void LoadInitialState();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node &node);
	bool Load(pugi::xml_node &node);

	//void SaveInitial();
	bool Restart();

	void UpdateAll(float dt, bool run);

	//Creators

	Entity*	 CreateEntity(entityType type, iPoint pos);

	void CreatePlayerColliders();
	void DeletePlayerColliders();
	
	void OnCollision(Collider * c1, Collider * c2);



public:
	
	SDL_Texture*	enemyTex = nullptr;
	SDL_Texture*	playerTex = nullptr;
	p2SString		enemyPath;
	p2SString		playerPath;
	p2List<Entity*> entities;	

	Floater		    floaterinfo;
	Roller			rollerinfo;
	Player			playerinfo;

	Entity*			player_ref = nullptr;
	j1Timer		    timer_death;
	bool			is_started;
};

