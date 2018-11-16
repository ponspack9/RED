#include "j1App.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "Entity.h"
#include "Static.h"

Static::Static(iPoint pos, Entity* e) :Entity(type)
{
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = e->speed;

	health = e->health;
	alive = e->alive;
	vision_range = e->vision_range;
}

Static::~Static()
{

}

bool Static::Update(float dt)
{
	return true;
}

bool Static::PostUpdate()
{
	return true;
}

bool Static::CleanUp()
{
	return true;
}

bool Static::Save(pugi::xml_node & node, const p2List<Entity*>* entities) const
{
	LOG("Saving floater stats");

	pugi::xml_node floater_node = node.append_child("static");

	p2List_item<Entity*>* item = entities->start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == STATIC)
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

bool Static::Load(pugi::xml_node & node, p2List<Entity*>* entities)
{
	LOG("Loading floater stats");

	pugi::xml_node floater_node = node.append_child("static");

	p2List_item<Entity*>* item = entities->start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == STATIC)
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
