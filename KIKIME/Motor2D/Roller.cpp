#include "Roller.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "Entity.h"


Roller::Roller(iPoint pos, Entity* e, SDL_Texture* sprites) : Entity(type)
{
	name.create("roller");

	this->sprites = sprites;
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = e->speed;

	idle = e->idle;
	health = e->health;
	alive = e->alive;
	vision_range = e->vision_range;

	current_animation = &idle;
	LOG("Roller Created");
	LOG("pos %d, %d", position.x, position.y);
}

Roller::~Roller()
{

}

bool Roller::Update(float dt)
{
	return true;
}

bool Roller::PostUpdate()
{
	Draw();
	return true;
}

bool Roller::CleanUp()
{
	return true;
}

void Roller::Draw()
{
	App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0);
}

void Roller::Move()
{

}

void Roller::Shoot()
{

}

bool Roller::UpdateLogic()
{
	return false;
}
