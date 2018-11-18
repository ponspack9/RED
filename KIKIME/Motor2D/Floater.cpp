#include "j1App.h"
#include "Floater.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"

Floater::Floater(iPoint pos,Entity* e,entityType type) : Entity(type)
{
	name.create("floater");
	
	this->type = type;
	return_origin = false;

	position = pos;
	initial_pos = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = { 0,0 };
	speed_mult = e->speed;

	idle = e->idle;
	follow = e->follow;
	health = e->health;
	alive = e->alive;
	vision_range = e->vision_range;

	current_animation = &idle;
	LOG("Floater Created");
	LOG("pos %d, %d",position.x,position.y);

	collider = App->collision->AddCollider(rect, COLLIDER_DEATH);
}

Floater::~Floater()
{

}


//This is called 5 times per second / called every ~15 frames
bool Floater::UpdateLogic(iPoint pos)
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(pos.x + App->render->camera.x, pos.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);

	if (p.DistanceTo(a) >= vision_range || !App->pathfinding->IsWalkable(a)) {
		return_origin = true;
		a = App->render->ScreenToWorld(initial_pos.x + App->render->camera.x,initial_pos.y + App->render->camera.y);
		a = App->map->WorldToMap(a.x, a.y);
		first_iteration = true;
	}

	if (App->pathfinding->CreatePath(p, a) == -1) {
		//origin not walkable
		speed = -speed;
	}
	path = App->pathfinding->GetLastPathNotConst();

	//Blit path to screen
	if (App->debug->show_colliders){
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
		}
	}

	//Set the speed of the enemy
	if (path->Count() > 1 && a != p) {
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

		if (speed.x > 0) {
			desired_position = App->map->MapToWorld(p.x + 1, p.y);
		}
		else if (speed.y > 0) {
			desired_position = App->map->MapToWorld(p.x, p.y + 1);
		}
		/*if (path->Count() > 2) {
			next_speed = iPoint(path->At(2)->x, path->At(2)->y) - iPoint(path->At(1)->x, path->At(1)->y);
		}
		else { next_speed = { 0,0 }; }*/
	}
	else { 
		first_iteration = true;
		speed = { 0,0 }; }
	if (return_origin) {
		current_animation = &idle;
	}
	else {
		current_animation = &follow;
	}
	path->Clear();
	return_origin = false;

	return false;
}