#pragma once
#include "j1Module.h"

#define MAX_ENEMIES 100

class EntityManager;

class Entity
{
public:

	enum entityType
	{
		ENEMY,
		PLAYER,
		NO_TYPE
	};

	enum enemyType
	{
		FLOATING,
		ROLLING,
		STOPPED
	};

public:

	Entity();
	Entity(entityType type);

	~Entity();

	virtual bool Awake(pugi::xml_node &config) { return true; }
	virtual bool Start() { return true; }
	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool CleanUp() { return true; }
	virtual bool Save(pugi::xml_node & node) const { return true; }
	virtual bool Load(pugi::xml_node & node) { return true; }

	void Draw();
	void OnCollision(Collider* c1, Collider* c2);

	const Collider* GetCollider() const;
	 //Mix_Chunk* Explosion = nullptr;


public:


	int						health;
	bool					alive;
	int						vision_range;
	fPoint					speed;
	p2List<iPoint>			spawns;
	p2List<SDL_Rect>		rect;	
	iPoint					position;

	Animation idle;
	Animation follow;
	Animation die;
	//Animation shoot;

	enemyType	enemy_type;
	entityType	type;

	bool first_iteration = true;

	iPoint desired_position;
	Animation* current_animation = nullptr;

	Collider* collider;

	p2SString texture_path;
	SDL_Texture* sprites;
	
	float def_anim_speed_enem;
};