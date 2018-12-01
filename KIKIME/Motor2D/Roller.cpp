#include "Roller.h"
#include "j1Pathfinding.h"
#include "j1Map.h"

bool Roller::UpdateLogic(iPoint pos)
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(pos.x + App->render->camera.x, pos.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);

	if (p.DistanceTo(a) >= vision_range || !App->pathfinding->IsWalkable(a)) {
		return_origin = true;
		a = App->render->ScreenToWorld(initial_pos.x + App->render->camera.x, initial_pos.y + App->render->camera.y);
		a = App->map->WorldToMap(a.x, a.y);
		first_iteration = true;
	}

	//if (!App->pathfinding->IsWalkable(a) || App->pathfinding->IsWalkable({ a.x,a.y + 2 })) {
	//	//LOG("a [%d,%d]", a.x, a.y);
	//	speed = { 0,0 };
	//	return false;
	//}

	if (App->pathfinding->CreatePath(p, a, true) == -1) {
		//origin not walkable
		speed = -speed;
	}
	//App->pathfinding->CreatePath(p, a, true);
	path = App->pathfinding->GetLastPathNotConst();
	//Blit path to screen
	if (App->debug->show_colliders)
		for (uint i = 0; i < path->Count(); ++i)
		{
			iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
			App->render->Blit(App->debug->debug_tex, pos.x, pos.y);
		}

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
	}
	else {
		first_iteration = true;
		speed = { 0,0 };
	}
	//else { speed = { 0,0 }; }
	path->Clear();
	return_origin = false;
	return true;

}


