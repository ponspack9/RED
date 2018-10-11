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

	position = { 50,50 };
	speed = 3;
	return true;
}

bool j1Player::Start()
{

	return true;
}

bool j1Player::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		position.y -= speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		position.y += speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		position.x += speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		position.x -= speed;
	}
	player_collider->SetPos(position.x, position.y);
	Draw();
	return true;
}

void j1Player::Draw()
{
	

}


