#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "p2Point.h"
#include "j1Player.h"


j1Player::j1Player()
{
	
}


j1Player::~j1Player()
{
}

bool j1Player::Awake(pugi::xml_node & config)
{
	//This need to be loaded from config, in the player child
	player_rect = { 0,0,64,64 };
	player_collider = App->collision->AddCollider(player_rect,COLLIDER_PLAYER, this);
	return true;
}

bool j1Player::Start()
{

	return true;
}

bool j1Player::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
	{
		
	}

	return true;
}

void j1Player::Draw()
{

}


