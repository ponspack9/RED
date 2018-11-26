#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "p2Log.h"
#include "Player.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include "Player.h"

Player::Player(iPoint pos, Entity * e, SDL_Texture * sprites,entityType type) : Entity(type)
{
	name.create("player");

	this->graphics = sprites;
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = e->speed;
	gravity = e->gravity;
	
	idle = e->idle;
	fall = e->fall;
	walk = e->walk;
	death = e->death;
	jump = e->jump;
	doublejump = e->doublejump;
	god = e->god;

	current_animation = &idle;
	alive = true;
}

Player::~Player()
{

}

bool Player::PreUpdate()
{
	BROFILER_CATEGORY("Player->PreUpdate", Profiler::Color::BlueViolet);
	if (alive) {
		
		//Jump
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			want_up = true;
		}
		//Smash
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			want_down = true;
		}
		//Left
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			want_left = true;

		}
		//Right
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			
			want_right = true;
		}
	}
		return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player->Update", Profiler::Color::BlueViolet);
	if (want_right) 
	{
		iPoint pos = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y);
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y + 32);
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y + collider->rect.h);
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3))		position.x += (int)(speed.x * dt);
		want_right = false;
	}
	else if (want_left)
	{
		iPoint pos = App->render->ScreenToWorld(position.x + App->render->camera.x - (int)(speed.x * dt), position.y + App->render->camera.y);
		pos = App->map->WorldToMap(pos.x, pos.y); 
		iPoint pos2 = App->render->ScreenToWorld(position.x + App->render->camera.x - (int)(speed.x * dt), position.y + App->render->camera.y + 32);
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + App->render->camera.x - (int)(speed.x * dt), position.y + App->render->camera.y + collider->rect.h);
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3))	position.x -= (int)(speed.x * dt);
		want_left = false;
	}
	if (want_up)
	{
		iPoint pos = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y - (int)(speed.y * dt));
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + App->render->camera.x + 32, position.y + App->render->camera.y - (int)(speed.y * dt));
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + App->render->camera.x + collider->rect.w, position.y + App->render->camera.y  - (int)(speed.y * dt));
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3))	position.y -= (int)(speed.y * dt);
		want_up = false;
	}
	else if (want_down)
	{
		iPoint pos = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + App->render->camera.x + 32, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + App->render->camera.x + collider->rect.w, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3))	position.y += (int)(speed.y * dt);
		want_down = false;
	}
	
	
	
	
	
	
	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player->PostUpdate", Profiler::Color::BlueViolet);
	current_animation = &idle;
	if (want_right) {
		current_animation = &walk;
	}
	return true;
}


bool Player::MovePlayer(float vel_x, float vel_y, float dt)
{
	return true;
}


void Player::Move()
{
				
}


bool Player::Jump()
{
	return true;
}

bool Player::DoubleJump()
{
	return true;
	
}

void Player::MoveFree()
{
	
}

void Player::PlayerAnimations()
{
	
}