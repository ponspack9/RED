#include "j1App.h"
#include "Floater.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "Entity.h"
#include "j1EntityManager.h"


Floater::Floater(iPoint pos) : Entity(type)
{
	this->type = type;
	position = pos;
}

Floater::~Floater()
{

}

bool Floater::Update(float dt)
{
	bool ret = true;


	return ret;
}

bool Floater::PostUpdate()
{
	return true;
}

bool Floater::CleanUp()
{
	return true;
}

bool Floater::Save(pugi::xml_node & node, const p2List<Entity*>* entities) const
{
	LOG("Saving floater stats");

	pugi::xml_node floater_node = node.append_child("floater");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == FLOATER)
		{
			floater_node.append_child("alive").append_attribute("value") = item->data->alive;
			floater_node.append_child("health").append_attribute("hp") = item->data->health;
			
			if (!item->data->alive)
				continue;
			else
			{
				floater_node.append_child("position");
				floater_node.append_attribute("x") = item->data->position.x;
				floater_node.append_attribute("y") = item->data->position.y;
			}
		}
	}
	return true;
}

bool Floater::Load(pugi::xml_node & node, p2List<Entity*>* entities)
{
	LOG("Loading floater stats");

	pugi::xml_node floater_node = node.append_child("floater");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == FLOATER)
		{
			item->data->alive = floater_node.child("alive").attribute("value").as_bool();
			item->data->health = floater_node.child("health").attribute("hp").as_int();

			if (!item->data->alive)
				continue;
			else
			{
				item->data->position.x = floater_node.child("position").attribute("x").as_int();
				item->data->position.y = floater_node.child("position").attribute("y").as_int();
			}
		}
	}
	return false;
}

void Floater::Shoot() 
{
	//App->particles->AddParticle(App->particles->Eshot1, position.x, position.y + animation->GetCurrentFrame().h / 2, COLLIDER_ENEMY_SHOT);
}

void Floater::Move()
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(App->player->position.x + App->render->camera.x, App->player->position.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);

	App->pathfinding->CreatePath(p, a);
	p2DynArray<iPoint>* path = App->pathfinding->GetLastPathNotConst();
	if (first_iteration) {
		speed = iPoint(path->At(1)->x, path->At(1)->y) - p;
		first_iteration = false;
	}
	//Blit path to screen
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
	}

	//Move the enemy towards player
	if (path->Count() > 1 && App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT/* && path->Count() < can_see*/) {

		if (desired_position == position) 	speed = iPoint(path->At(1)->x, path->At(1)->y) - p;

		desired_position = App->map->MapToWorld(p.x, p.y);

		if (speed.x > 0 && speed.y == 0) {
			desired_position = App->map->MapToWorld(p.x +1, p.y);
		}
		else if (speed.y > 0 && speed.x == 0) {
			desired_position = App->map->MapToWorld(p.x, p.y +1);
		}
		//Happens only when diagonal movemnt implemented
		/*if (speed.y > 0 && speed.x > 0) {
			desired_position = App->map->MapToWorld(p.x+1, p.y + 1);
		}*/
		position += speed;
	}
	path->Clear();
}