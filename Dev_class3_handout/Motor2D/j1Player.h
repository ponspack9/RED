#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "j1Collision.h"

class j1Player : public j1Module
{

public:

	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool Update(float dt);
	//bool CleanUp();

	void Draw();
	void Move();
	bool Jump();

	bool Save(pugi::xml_node& node);
	bool Load(pugi::xml_node& node);

	void OnCollision(Collider* c1, Collider* c2);

	void OnCollisionLine(Collider* c, int x1, int y1, int x2, int y2);

public:

	bool have_collided;
	bool on_top;
	bool on_floor;
	bool colliding_floor;
	bool dead;
	bool move_left;
	bool move_right;
	bool is_jumping;
	
	int max_speed_y;
	float gravity;
	float jumpspeed;

	fPoint position;
	iPoint init_pos1;
	iPoint init_pos2;

	fPoint speed;
	SDL_Rect player_rect;
	bool godmode = false;

	Collider* player_collider;
	Collider* shade_collider;

	COLLIDER_TYPE last_collision;
};

#endif 