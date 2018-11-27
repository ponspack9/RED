#pragma once

#include "j1Module.h"
#include "p2Defs.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Debug.h"

#define MAX_ENEMIES 100

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

	Entity(){}

	Entity(entityType type) {}

	~Entity(){}

	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { 
		position.x += (int)(speed.x * dt);
		position.y += (int)(speed.y * dt);
		return true; 
	}
	virtual bool PostUpdate() { return true; }

	virtual void Draw(SDL_Texture *sprites) {
		App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0);
		if (collider) collider->SetPos(position.x, position.y);
	}

	virtual bool UpdateLogic(iPoint pos) { return true; }

public:

	////////// GENERAL /////////


	p2SString		name;

	bool			alive;

	int				health;
	int				vision_range;

	SDL_Rect		rect;	

	iPoint			speed;
	iPoint			speed_mult;
	iPoint			position;
	iPoint			initial_pos;

	Collider*		collider;

	entityType		type;

	//Mix_Chunk*	die_FX;

	/////////////// ENEMIES /////////////////

	float			def_anim_speed_enem;

	bool			first_iteration = true;
	bool			return_origin;

	Animation		follow;

	iPoint			desired_position;

	///////////////// PLAYER ///////////////////

	SDL_Rect		anim_rect;
	Animation*		current_animation = nullptr;
	Animation		idle;
	Animation		walk;
	Animation		jump;
	Animation		doublejump;
	Animation		fall;
	Animation		death;
	Animation		god;

	float			def_anim_speed;
	float			gravity;
	float			god_speed;

	bool			level_finished;

	//Animations

	bool			dead;
	bool			god_mode;


};