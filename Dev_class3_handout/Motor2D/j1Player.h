#ifndef __j1PLAYER_H__
#define __j1PLAYER_H__

#include "SDL/include/SDL.h"
#include "j1Module.h"


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

public:

	SDL_Rect player_rect;
	iPoint position;
	int speed;


	Collider* player_collider;
};

#endif 