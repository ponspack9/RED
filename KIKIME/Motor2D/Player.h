#pragma once
#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Entity.h"
#include "j1Audio.h"

class Player : public Entity
{

public:

	Player() {}
	Player(iPoint pos, Player* e);
	~Player();

	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	
	void Move(float dt);
	void MoveFree(float dt);
	void Jump();
	void Smash();
	void PlayerAnimations(float dt);

	void Draw(SDL_Texture *sprites);	

	void ResetPlayer();

public:

	bool		want_right;
	bool		want_left;
	bool		want_up;
	bool		want_down;
	bool		going_down;

	bool		jumping;
	bool		double_jumping;
	bool		smashing;
	bool		god_mode;
	bool		moving_left;
	bool		moving_right;
	bool		level_finished;

	int			jump_speed;
	int			smash_speed;
	int			lifes;

	float		gravity;
	float		god_speed;
	float		max_speed;


	Animation	walk;
	Animation	jump;
	Animation	doublejump;
	Animation	fall;
	Animation	death;
	Animation	god;
	Animation   smash;
};