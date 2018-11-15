#include "Roller.h"

Roller::Roller(int x, int y)
{

}

Roller::~Roller()
{

}

bool Roller::Update(float dt)
{
	return false;
}

bool Roller::PostUpdate()
{
	return false;
}

bool Roller::CleanUp()
{
	return false;
}

bool Roller::Save(pugi::xml_node & node) const
{
	LOG("Saving floater stats");

	pugi::xml_node floater_node = node.append_child("roller");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == ROLLER)
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

bool Roller::Load(pugi::xml_node & node)
{
	LOG("Loading floater stats");

	pugi::xml_node floater_node = node.append_child("roller");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
	for (item; item != nullptr; item = item->next)
	{
		if (item->data->type == ROLLER)
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
