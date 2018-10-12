#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "SDL/include/SDL.h"
#include "j1Module.h"

struct Player
{
	iPoint position;
	fPoint speed;
	SDL_Rect player_rect;
	bool godmode = false;
};


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
	void Jump();

	bool Save(pugi::xml_node& node);
	bool Load(pugi::xml_node& node);

public:

	bool on_top=false;
	bool on_floor=true;
	bool dead = false;
	bool move_left;
	bool move_right;
	bool is_jumping;
	
	float aux_speed_y;
	int aux_ypos;

	Collider* player_collider;

	Player data;
	pugi::xml_node player_node
};

#endif 