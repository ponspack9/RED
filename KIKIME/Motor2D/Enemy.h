#pragma once
#include "Entity.h"
#include "p2DynArray.h"


struct SDL_Texture;
struct Collider;

class Enemy : public Entity {

protected:

	p2DynArray<iPoint>* path;

public:
	Enemy() {}
	Enemy(iPoint pos, Enemy* e) : Entity(pos, e){
		
		initial_pos = pos;
		speed = { 0,0 };
		speed_mult = e->speed;

		follow = e->follow;
		vision_range = e->vision_range;

		// initializing
		first_iteration = true;
		return_origin = false;
		desired_position = { 0,0 };

	}

	~Enemy() {}

public:

	int				vision_range;

	bool			first_iteration;
	bool			return_origin;

	Animation		follow;

	iPoint			desired_position;
	iPoint			speed_mult;
	iPoint			initial_pos;
};

