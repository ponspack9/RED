#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "j1Collision.h"
#include "Animation.h"

class j1Player : public j1Module
{

public:

	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Draw();
	bool MovePlayer(int vel_x, int vel_y);
	void Move();
	void MoveFree();
	bool Jump();
	void PlayerAnimations();

	bool Save(pugi::xml_node& node);
	bool Load(pugi::xml_node& node);

	void OnCollision(Collider* c1, Collider* c2);

	void OnCollisionLine(Collider* c, int x1, int y1, int x2, int y2);

public:
	p2SString texture_path;
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	Animation idle;
	Animation walk;		
	Animation jump;		
	Animation death;
	float def_anim_speed;
	SDL_Rect anim_rect;

	bool have_collided;
	bool level_finished;
	bool on_floor;
	bool on_wall;
	bool colliding_floor;
	bool dead;
	bool move_left;
	bool move_right;
	bool is_jumping;
	float dx = 0;
	float dy = 0;
	int max_speed_y;
	float gravity;
	float jumpspeed;
	bool top, bottom, left, right;
	fPoint position;
	iPoint init_pos1;
	iPoint init_pos2;

	fPoint speed;
	SDL_Rect player_rect;
	bool godmode = false;

	Collider* player_collider;
	Collider* wall;

	int r, g, b;
};

#endif 