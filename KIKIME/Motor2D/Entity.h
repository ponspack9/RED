#pragma once

#include "j1Module.h"
#include "p2Defs.h"
#include "Animation.h"
#include "p2Point.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Collision.h"

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

	Entity(entityType type)
	{		
	}

	~Entity()
	{
	}

	virtual bool PreUpdate() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual bool PostUpdate() { return true; }

	virtual void Draw() {}
	void OnCollision(Collider* c1, Collider* c2) {}

	const Collider* GetCollider() const { return collider; }
	 //Mix_Chunk* Explosion = nullptr;

	virtual bool UpdateLogic() { return true; }

public:

	////////// GENERAL /////////

	p2SString		name;
	SDL_Rect		rect;	
	iPoint			speed;
	iPoint			position;
	Collider*		collider;
	entityType		type;

	/////////////// ENEMIES /////////////////

	int			 health;
	bool		 alive;
	int			 vision_range;
	Animation	 follow;
	//Animation  shoot;
	float		 def_anim_speed_enem;

	bool		 first_iteration = true;

	iPoint		 desired_position;

	///////////////// PLAYER ///////////////////
	//fPoint position;
	//fPoint speed;
	Animation*	 current_animation = nullptr;
	Animation	 idle;
	Animation	 walk;
	Animation	 jump;
	Animation	 doublejump;
	Animation	 fall;
	Animation	 death;
	Animation	 god;
	float        def_anim_speed;
	SDL_Rect	 anim_rect;
	float		 dx = 0;
	float		 dy = 0;
	float		 falling_y;
	int			 max_speed_y;
	float		 gravity;
	float		 jumpspeed;


	//Animations
	bool	level_finished;
	bool	on_floor;
	bool	dead;
	bool	move_left;
	bool	move_right;
	bool	is_jumping;
	bool	is_falling;
	bool	djump;
	bool	aux_djump;
	bool	top, bottom, left, right;
	bool	can_left, can_right;
	bool	godmode = false;

	//NEW approach
	bool	can_move_right;
	bool	can_move_left;
	bool	can_move_up;
	bool	can_move_down;

	bool	vertical_collided;
	bool	horizontal_collided;

	Collider*	collider_ray_right;
	Collider*	collider_ray_left;
	Collider*	collider_ray_up;
	Collider*	collider_ray_down;
	Collider*	collider_identifier;

	int				collider_offset = 0;
};