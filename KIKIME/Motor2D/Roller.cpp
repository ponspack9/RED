#include "Roller.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "Entity.h"



Roller::Roller(iPoint pos, Entity* e,entityType type) : Entity(type)
{
	name.create("roller");

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
	LOG("Roller Created");
	LOG("pos %d, %d", position.x, position.y);

	App->collision->AddCollider(rect, COLLIDER_DEATH);
}

Roller::~Roller()
{

}

bool Roller::Update(float dt)
{
	position += speed;
	return true;
}

bool Roller::PostUpdate()
{
	return true;
}

bool Roller::UpdateLogic()
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(App->player->position.x + App->render->camera.x, App->player->position.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);
	/*if (App->pathfinding->IsWalkable(p) && App->pathfinding->IsWalkable({p.x,p.y+1})) {
		speed = -speed;

	}*/
	if (!App->pathfinding->IsWalkable(a) || App->pathfinding->IsWalkable({ a.x,a.y + 2 })) {
		LOG("a [%d,%d]", a.x, a.y);
		speed = { 0,0 };
		return false;
	}
	if (p.DistanceTo(a) >= vision_range) {
		speed = { 0,0 };
		return false;
	}

	App->pathfinding->CreatePath(p, a, true);
	path = App->pathfinding->GetLastPathNotConst();
	//Blit path to screen
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
	}

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
	//else { speed = { 0,0 }; }
	path->Clear();
	return true;

}


