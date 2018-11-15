#pragma once
#include "j1Module.h"
#include "p2Defs.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1App.h"

#define MAX_ENEMIES 100

class EntityManager;
class Animation;
struct SDL_Texture;

enum entityType
{
	FLOATER,
	ROLLER,
	STATIC,
	PLAYER,
	NO_TYPE
};

class Entity
{
public:


public:

	Entity();
	Entity(entityType type);

	~Entity();

	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }
	virtual bool CleanUp() { return true; }
	virtual bool Save(pugi::xml_node & node) const { return true; }
	virtual bool Load(pugi::xml_node & node) { return true; }

	void Draw(float dt);
	void OnCollision(Collider* c1, Collider* c2);

	const Collider* GetCollider() const;
	 //Mix_Chunk* Explosion = nullptr;


public:


	int						health;
	bool					alive;
	int						vision_range;
	iPoint					speed;
	p2List<iPoint>			spawns;
	p2List<SDL_Rect>		rect;	
	iPoint					position;

	Animation idle;
	Animation follow;
	Animation die;
	//Animation shoot;

	entityType	type;

	bool first_iteration = true;

	iPoint desired_position;
	Animation* current_animation = nullptr;

	Collider* collider;

	p2SString texture_path;
	SDL_Texture* sprites;
	
	float def_anim_speed_enem;
};