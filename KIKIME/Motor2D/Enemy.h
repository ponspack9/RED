#pragma once

#include "p2Point.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "SDL\include\SDL_timer.h"


struct SDL_Texture;
struct Collider;

class Enemy {

protected:
	Collider* collider = nullptr;
	uint current_time;
	uint last_time; 


public:
	Enemy(int x, int y);
	~Enemy(); //virtual

	const Collider* GetCollider() const;

	virtual void Move() {};
	virtual void Draw(SDL_Texture* sprites);
	virtual void PowerUp();

public:

	int health;
	bool alive = true;
	bool first_iteration = true;
	int can_see = 15;

	iPoint speed;
	iPoint last_velocity;
	iPoint position;
	iPoint desired_position;
	Animation* animation = nullptr;
};

