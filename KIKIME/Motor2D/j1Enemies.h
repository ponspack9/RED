#pragma once
#include "j1Module.h"

#define MAX_ENEMIES 100


enum ENEMY_TYPES
{
	NO_TYPE,
	ENEMY_AIR
};


struct EnemyInfo
{
	ENEMY_TYPES type = ENEMY_TYPES::NO_TYPE;
	int x, y;
};

class Enemy;

class j1Enemies : public j1Module
{
public:

	j1Enemies();
	~j1Enemies();

	bool Awake(pugi::xml_node &config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	bool AddEnemy(ENEMY_TYPES type, int x, int y);



	//Mix_Chunk* Explosion = nullptr;
	SDL_Rect enemy_air_rect;
	Enemy* enemies[MAX_ENEMIES];

private:

	void SpawnEnemy(const EnemyInfo& info);

private:
	p2SString texture_path;

	EnemyInfo queue[MAX_ENEMIES];
	SDL_Texture* sprites;

	
};