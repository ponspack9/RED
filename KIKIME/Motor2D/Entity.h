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
	COIN,

	NO_TYPE
};

class Entity
{
public:

	Entity(){}

	Entity(iPoint pos, Entity * e) {

		type = e->type;
		rect = { pos.x,pos.y,e->rect.w,e->rect.h };

		switch (type)
		{
		case FLOATER:
			name.create("floater");
			break;
		case ROLLER:
			name.create("roller");
			break;
		case STATIC:
			name.create("static");
			break;
		case PLAYER:
			name.create("player");
			break;
		case COIN:
			name.create("coin");
			break;
		case NO_TYPE:
			LOG("ERROR creating enemy, no type parsed");
			break;
		default:
			LOG("ERROR CREATING ENEMY");
			break;
		}

		position = pos;
		speed = e->speed;
		alive = e->alive;

		idle = e->idle;
		current_animation = &idle;
		def_anim_speed = e->def_anim_speed;
		
		LOG("Entity %s created at [%d, %d]", name.GetString(), position.x, position.y);
	}

	~Entity(){}

	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { 
		position.x += (int)(speed.x * dt);
		position.y += (int)(speed.y * dt);
		return true; 
	}
	virtual bool PostUpdate() { return true; }

	virtual void Draw(SDL_Texture *sprites) {
		if (App->pause) {
			current_animation->speed = 0;
		} else current_animation->speed = def_anim_speed * App->dt;
		App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0);
		if (collider != nullptr) collider->SetPos(position.x, position.y);
		//else LOG("NO COLLIDER");
	}

	virtual bool UpdateLogic(iPoint pos) { return true; }

public:

	p2SString		name;

	bool			alive;

	float			def_anim_speed;

	SDL_Rect		rect;

	iPoint			speed;
	iPoint			position;

	Collider*		collider;

	entityType		type;

	Animation*		current_animation = nullptr;
	Animation		idle;

	//Mix_Chunk*	die_FX;
};