#include "Floater.h"
#include "j1Pathfinding.h"
#include "j1Map.h"


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
			App->render->Blit(App->debug->debug_tex, pos.x, pos.y);
		}
	}

	//Set the speed of the enemy
	if (path->Count() > 1 && a != p) {
		if (first_iteration) {
			speed = iPoint(path->At(1)->x, path->At(1)->y) - p;
			speed = speed * speed_mult;
			first_iteration = false;
		}
		

		if (speed.x < 0 && desired_position.x >= position.x) {
			speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
			LOG("Speed changedX-:[%d,%d]", speed.x, speed.y);
		}
		else if (speed.x > 0 && desired_position.x <= position.x) 	{
			speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
			LOG("Speed changedX+:[%d,%d]", speed.x, speed.y);
		}
		if (speed.y < 0 && desired_position.y >= position.y) 		{
			speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
			LOG("Speed changedY-:[%d,%d]", speed.x, speed.y);
		}
		else if (speed.y > 0 && desired_position.y <= position.y) 	{
			speed = (iPoint(path->At(1)->x, path->At(1)->y) - p) * speed_mult;
			LOG("Speed changedY+:[%d,%d]", speed.x, speed.y);
		}
		desired_position = App->map->MapToWorld(p.x, p.y);

		if (speed.x > 0) {
			desired_position = App->map->MapToWorld(p.x + 1, p.y);
		}
		else if (speed.y > 0) {
			desired_position = App->map->MapToWorld(p.x, p.y + 1);
		}

		LOG("position [%d,%d] D:[%d,%d] S:[%d,%d]", position.x, position.y, desired_position.x, desired_position.y, speed.x, speed.y);
		
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