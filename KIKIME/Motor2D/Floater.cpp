#include "j1App.h"
#include "Floater.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"


Floater::Floater(iPoint pos,Entity* e) : Entity(type)
{
	name.create("floater");
	
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = { 0,0 };
	speed_mult = e->speed;

	idle = e->idle;
	health = e->health;
	alive = e->alive;
	vision_range = e->vision_range;

	current_animation = &idle;
	LOG("Floater Created");
	LOG("pos %d, %d",position.x,position.y);

	App->collision->AddCollider(rect, COLLIDER_DEATH);
}

Floater::~Floater()
{

}

bool Floater::Update(float dt)
{
	position += speed;
	return true;
}

bool Floater::PostUpdate()
{
	return true;
}

bool Floater::CleanUp()
{
	return true;
}

void Floater::Shoot() 
{
	//App->particles->AddParticle(App->particles->Eshot1, position.x, position.y + animation->GetCurrentFrame().h / 2, COLLIDER_ENEMY_SHOT);
}

//This is called 5 times per second / called every ~15 frames
bool Floater::UpdateLogic()
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(App->player->position.x + App->render->camera.x, App->player->position.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);

	if (p.DistanceTo(a) >= vision_range) {
		speed = { 0,0 };
		return false;
	}

	App->pathfinding->CreatePath(p, a);
	path = App->pathfinding->GetLastPathNotConst();
	//Blit path to screen
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
	}

	//Move the enemy towards player
	if (path->Count() > 1) {
		
		if (first_iteration) {
			speed = iPoint(path->At(1)->x, path->At(1)->y) - p;
			speed = speed * speed_mult;
			first_iteration = false;
		}

		if (speed.x < 0 && desired_position.x >= position.x) 		speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
		else if (speed.x > 0 && desired_position.x <= position.x) 	speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
		if (speed.y < 0 && desired_position.y >= position.y) 		speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
		else if (speed.y > 0 && desired_position.y <= position.y) 	speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;

		desired_position = App->map->MapToWorld(p.x, p.y);

		if (speed.x > 0 && speed.y == 0) {
			desired_position = App->map->MapToWorld(p.x + 1, p.y);
		}
		else if (speed.y > 0 && speed.x == 0) {
			desired_position = App->map->MapToWorld(p.x, p.y + 1);
		}		
	}
	path->Clear();

	return false;
}