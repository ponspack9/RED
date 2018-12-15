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
#include "Coin.h"
#include "j1Timer.h"



struct SDL_Texture;
struct Mix_Chunk;

class j1EntityManager : public j1Module 
{

private:

	float	current_time;
	float	update_cycle;
	bool	do_update;

public:
	j1EntityManager();
	~j1EntityManager(); //virtual

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	//Controllers
	void CreateEntities(int player_lifes,iPoint player_pos);
	void CleanEntities();
	bool Restart(int player_lifes, iPoint player_pos);

	bool Save(pugi::xml_node &node);
	bool Load(pugi::xml_node &node);

	void UpdateAll(float dt, bool run);

	void Draw();

	//Creators

	Entity*	 CreateEntity(entityType type, iPoint pos, coinType coin_type = coinType::NOTYPE);
	
	void OnCollision(Collider * c1, Collider * c2);

	Entity * FindEntityByCollider(Collider * c);

	Coin* FindCoinByCollider(Collider* c);

	void ChangeUIAnimation(Coin* c);

	float GetAnimTimer();

public:
	
	SDL_Texture*	enemyTex = nullptr;
	SDL_Texture*	playerTex = nullptr;
	SDL_Texture*	otherTex = nullptr;
	p2SString		enemyPath;
	p2SString		playerPath;
	p2SString		otherPath;
	p2List<Entity*> entities;	

	Floater		    floaterinfo;
	Roller			rollerinfo;
	Player			playerinfo;
	Player*			player_ref = nullptr;
	Coin			green_diamond;
	Coin			blue_diamond;
	Coin			heart;

	j1Timer		    timer_death;
	bool			is_started;
	int				score;
	int				score_powUp;
	int				aux_score;
	int				blue_counter;
	int				green_counter;

	p2List<Mix_Chunk*> entities_fx;
};

