#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "j1EntityManager.h"
#include "Entity.h"

j1EntityManager::j1EntityManager()
{
	name.create("entitymanager");
}

j1EntityManager::~j1EntityManager()
{
}

bool j1EntityManager::Awake(pugi::xml_node & config)
{
	//reading all info
	enemyPath = (config.child("enemypath").attribute("path").as_string());
	playerPath = (config.child("playerpath").attribute("path").as_string());

	update_cycle = config.child("updatems").attribute("time").as_float();

	pugi::xml_node f_enem = config.child("enemies").child("enemyair");

	floaterinfo->speed.x = f_enem.child("speed").attribute("x").as_float();
	floaterinfo->speed.y = f_enem.child("speed").attribute("y").as_float();

	floaterinfo->vision_range = f_enem.child("range").attribute("value").as_int();

	floaterinfo->health = f_enem.child("health").attribute("hp").as_int();

	floaterinfo->alive = f_enem.child("alive").attribute("value").as_bool();

	pugi::xml_node i = f_enem.child("spawn");

	p2List_item<SDL_Rect>* item1 = floaterinfo->rect.start;
	p2List_item<iPoint>* item2 = floaterinfo->spawns.start;

	for (i; i; i = i.next_sibling("spawn"))
	{
		item2->data.x = i.attribute("x").as_int();
		item1->data.x = item2->data.x;
		item2->data.y = i.attribute("y").as_int();
		item1->data.y = item2->data.y;

		item1->data.w = f_enem.child("size").attribute("w").as_int();
		item1->data.h = f_enem.child("size").attribute("h").as_int();

		item2 = item2->next;
		item1 = item1->next;
	}

	pugi::xml_node r_enem = config.child("enemies").child("enemyground");

	rollerinfo->speed.x = r_enem.child("speed").attribute("x").as_float();
	rollerinfo->speed.y = r_enem.child("speed").attribute("y").as_float();

	rollerinfo->vision_range = r_enem.child("range").attribute("value").as_int();

	rollerinfo->health = r_enem.child("health").attribute("hp").as_int();

	rollerinfo->alive = r_enem.child("alive").attribute("value").as_bool();

	pugi::xml_node i = r_enem.child("spawn");

	p2List_item<SDL_Rect>* item1 = rollerinfo->rect.start;
	p2List_item<iPoint>*   item2 = rollerinfo->spawns.start;

	for (i; i; i = i.next_sibling("spawn"))
	{
		item2->data.x = i.attribute("x").as_int();
		item1->data.x = item2->data.x;
		item2->data.y = i.attribute("y").as_int();
		item1->data.y = item2->data.y;

		item1->data.w = f_enem.child("size").attribute("w").as_int();
		item1->data.h = f_enem.child("size").attribute("h").as_int();

		item2 = item2->next;
		item1 = item1->next;
	}

	//reading animations

	return true;
}

bool j1EntityManager::Start()
{
	//ni papa dels errors en App
	enemyTex = App->tex->Load(enemyPath.GetString());
	playerTex = App->tex->Load(playerPath.GetString());

	return true;
}

bool j1EntityManager::Update(float dt)
{
	current_time += dt;
	if (current_time >= update_cycle)
	{
		do_update = true;
	}

	UpdateAll(dt,do_update);

	if (do_update)
	{
		current_time = 0.0f;
		do_update = false;
	}
	
	return true;
}

bool j1EntityManager::PostUpdate()
{
	bool ret = false;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->PostUpdate();
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = false;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->CleanUp();
	}

	return ret;
}

bool j1EntityManager::Save(pugi::xml_node & node) const
{
	bool ret = false;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->Save(node);
	}

	return ret;
}

bool j1EntityManager::Load(pugi::xml_node & node)
{
	p2List_item<Entity*>* item = entities.start;
	pugi::xml_node enemy = node.child("enemies");
	pugi::xml_node player = node.child("player");

	while (item != nullptr)
	{
		if (item->data->type == Entity::entityType::ENEMY)
		{
			item->data->Load(enemy);
		}
		if (item->data->type == Entity::entityType::PLAYER)
		{
			item->data->Load(player);
		}

		item = item->next;
	}
	return true;
}

void j1EntityManager::UpdateAll(float dt,bool run)
{
	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == Entity::entityType::PLAYER)
		{
			item->data->Update(dt);
		}
	}

	if (run)
	{
		for (item = entities.start; item != nullptr; item = item->next)
		{
			item->data->Update(dt);
		}
	}
}

Entity * j1EntityManager::CreateEntity(Entity::entityType type, iPoint pos)
{
	//falta crear classes especifiques
	static_assert(Entity::entityType::NO_TYPE == 2, "NO TYPE TO CREATE, CODE NEEDS UPDATE");
	Entity* entity = nullptr;
	switch (type)
	{
	case Entity::entityType::ENEMY:

		if (Entity::enemyType::FLOATING)
		{
			entity = new Floater(type, pos);
			break;
		}
		if(Entity::enemyType::ROLLING)
		{
			entity = new Roller(type, pos);
			break;
		}

	case Entity::entityType::PLAYER:

		entity = new Player(type, pos);
		break;
	}

	entities.add(entity);

	return entity;
}

bool j1EntityManager::DeleteEntity(Entity * entity)
{
	return entity->CleanUp();
}
