#include "Static.h"

Static::Static(int x, int y)
{

}

Static::~Static()
{

}

bool Static::Update(float dt)
{
	return false;
}

bool Static::PostUpdate()
{
	return false;
}

bool Static::CleanUp()
{
	return false;
}

bool Static::Save(pugi::xml_node & node) const
{
	LOG("Saving floater stats");

	pugi::xml_node floater_node = node.append_child("static");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
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

bool Static::Load(pugi::xml_node & node)
{
	LOG("Loading floater stats");

	pugi::xml_node floater_node = node.append_child("static");

	p2List_item<Entity*>* item = App->entitymanager->entities.start;
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
