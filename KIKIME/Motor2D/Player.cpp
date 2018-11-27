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
	speed.x = e->speed.x;
	speed.y = 0;
	jump_speed = -e->speed.y;
	
	god_speed = e->god_speed;
	gravity = e->gravity;
	
	idle = e->idle;
	fall = e->fall;
	walk = e->walk;
	death = e->death;
	jump = e->jump;
	doublejump = e->doublejump;
	god = e->god;
	def_anim_speed = e->def_anim_speed;

	current_animation = &idle;
	alive = true;
	jumping = false;
}

Player::~Player()
{

}

bool Player::PreUpdate()
{
	gravity_enabled = true;
	BROFILER_CATEGORY("Player->PreUpdate", Profiler::Color::BlueViolet);
	if (alive) {
		
		//Jump
		if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) || (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT))
		{
			if (!jumping) Jump();
			want_up = true;
		}
		//Smash //want_down only in god mode
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

	if (!god_mode) {
		Move(dt);
	}
	else {
		MoveFree(dt);
	}
	PlayerAnimations(dt);
	
	return true;
}

void Player::PlayerAnimations(float dt)
{
	if (!god_mode) {
		current_animation = &idle;
	}
	else {
		current_animation = &god;
	}

	current_animation->speed = def_anim_speed * dt;
}


bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player->PostUpdate", Profiler::Color::BlueViolet);
	
	return true;
}






bool Player::Jump()
{
	speed.y = jump_speed;
	jumping = true;

	return true;
}

bool Player::DoubleJump()
{
	return true;
	
}

void Player::Move(float dt)
{
	if (want_right)
	{
		iPoint pos = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y);
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y + 32);
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + collider->rect.w + App->render->camera.x + (int)(speed.x * dt), position.y + App->render->camera.y + collider->rect.h);
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3)) {
			position.x += (int)(speed.x * dt);
			//Animation
			//going_right = true;
		}
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

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3)) {
			position.x -= (int)(speed.x * dt);
			//Animation
			//going_left = true;
		}
		want_left = false;
	}

	//if (want_up)
	if (speed.y < 0) // going up
	{
		iPoint pos = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y + (int)(speed.y * dt));
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + App->render->camera.x + 32, position.y + App->render->camera.y + (int)(speed.y * dt));
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + App->render->camera.x + collider->rect.w, position.y + App->render->camera.y + (int)(speed.y * dt));
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3)) {
			position.y += (int)(speed.y * dt);
			//Animation
			//going_up = true;
		}
		else { speed.y = (int)(gravity * dt); }
		want_up = false;
	}
	//Always move down if gravity is true, and player can, obviously
	else //if (gravity_enabled)
	{
		iPoint pos = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos = App->map->WorldToMap(pos.x, pos.y);
		iPoint pos2 = App->render->ScreenToWorld(position.x + App->render->camera.x + 32, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos2 = App->map->WorldToMap(pos2.x, pos2.y);
		iPoint pos3 = App->render->ScreenToWorld(position.x + App->render->camera.x + collider->rect.w, position.y + App->render->camera.y + collider->rect.h + (int)(speed.y * dt));
		pos3 = App->map->WorldToMap(pos3.x, pos3.y);

		if (App->pathfinding->IsWalkable(pos) && App->pathfinding->IsWalkable(pos2) && App->pathfinding->IsWalkable(pos3)) {
			position.y += (int)(speed.y * dt);
			//Animation
			//going_down = true;
		}
		else {
			//position.y = pos.y - position.y + collider->rect.h -5;
			jumping = false;
		}
	}

	float max_speed = 400.0f;
	if (speed.y < max_speed) {
		int g = (int)(gravity * dt);
		speed.y += (int)(g);
		if (speed.y > max_speed) speed.y = max_speed;
	}
}

void Player::MoveFree(float dt)
{
	if (want_right)
	{
		position.x += (int)(god_speed * dt);
		want_right = false;
	}
	else if (want_left)
	{
		position.x -= (int)(god_speed * dt);
		want_left = false;
	}
	if (want_up)
	{
		position.y -= (int)(god_speed * dt);
		want_up = false;
	}
	else if (want_down)
	{
		position.y += (int)(god_speed * dt);
		want_down = false;
	}
}
