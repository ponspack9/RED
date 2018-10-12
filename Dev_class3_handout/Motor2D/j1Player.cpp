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
	pugi::xml_node player_node = config.child("player");

	data.position.x=player_node.child("player_rect").attribute("x").as_int();
	data.position.y=player_node.child("player_rect").attribute("y").as_int();

	data.player_rect.x = data.position.x;
	data.player_rect.y = data.position.y;
	data.player_rect.w = player_node.child("player_rect").attribute("width").as_int();
	data.player_rect.h = player_node.child("player_rect").attribute("height").as_int();
	
	player_collider = App->collision->AddCollider(data.player_rect,COLLIDER_PLAYER, this);

	data.speed.x = player_node.child("speed").attribute("scrollspeed").as_int();
	data.speed.y = player_node.child("speed").attribute("jumpspeed").as_int();
	
	return true;
}

bool j1Player::Start()
{

	return true;
}

bool j1Player::Update(float dt)
{
	Move();
		
	player_collider->SetPos(data.position.x, data.position.y);
	Draw();
	return true;
}

void j1Player::Draw()
{
	

}

void j1Player::Move()
{
	
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		move_right = true;
	
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		move_left = true;		
	
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		bool is_juming = true;
		
	}
	//if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	//{
	//	data.position.y += data.speed.y;
	//}

	if (is_jumping && on_floor) 
		Jump();
	
	if (move_right)
		data.position.x += data.speed.x;
	
	if (move_left)
		data.position.x -= data.speed.x;

	if (!is_jumping && !on_floor)
	{
		on_top = true;
		data.position.y += aux_speed_y;
		aux_speed_y += 0.1;
		if (aux_speed_y == data.speed.y)
		{
			data.position.y += aux_speed_y;
		}
	}
}

void j1Player::Jump()
{	
	on_top = false;
	on_floor = false;
	aux_speed_y = data.speed.y;
	aux_ypos = data.position.y;
	
	if (is_jumping && !move_left && !move_right)
	{
		data.position.y -= aux_speed_y;
		aux_speed_y -= 0.1;
		if (aux_speed_y == 0)
		{
			on_top = true;
			data.position.y += aux_speed_y;
			aux_speed_y += 0.1;
			if (aux_speed_y == data.speed.y)
			{
				data.position.y += aux_speed_y;
			}
		
		}
		
	}
	if (is_jumping && move_left && !move_right)
	{
		data.position.x -= data.speed.x;
		data.position.y -= aux_speed_y;
		aux_speed_y -= 0.1;
		if (aux_speed_y == 0)
		{
			on_top = true;
			data.position.y += aux_speed_y;
			aux_speed_y += 0.1;
			if (aux_speed_y == data.speed.y)
			{
				data.position.y += aux_speed_y;
			}

		}
	}
	if (is_jumping && !move_left && move_right)
	{
		data.position.x -= data.speed.x;
		data.position.y -= aux_speed_y;
		aux_speed_y -= 0.1;
		if (aux_speed_y == 0)
		{
			on_top = true;
			data.position.y += aux_speed_y;
			aux_speed_y += 0.1;
			if (aux_speed_y == data.speed.y)
			{
				data.position.y += aux_speed_y;
			}

		}
	}
	if (is_jumping && move_left && move_right)
	{
		data.position.y -= aux_speed_y;
		aux_speed_y -= 0.1;
		if (aux_speed_y == 0)
		{
			on_top = true;
			data.position.y += aux_speed_y;
			aux_speed_y += 0.1;
		}
		if (aux_speed_y == data.speed.y)
		{
			data.position.y += aux_speed_y;
		}
	}

}


