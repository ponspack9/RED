#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "j1EntityManager.h"
#include "j1Textures.h"
#include "Floater.h"
#include "Roller.h"
#include "Static.h"

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

	floaterinfo.def_anim_speed_enem = config.child("enemies").child("enemyanimations").child("speed").attribute("defaultAnimationSpeed").as_float();

	//floater
	pugi::xml_node f_enem = config.child("enemies").child("enemyair");

	floaterinfo.speed.x = f_enem.child("speed").attribute("x").as_float();
	floaterinfo.speed.y = f_enem.child("speed").attribute("y").as_float();

	floaterinfo.vision_range = f_enem.child("range").attribute("value").as_int();

	floaterinfo.health = f_enem.child("health").attribute("hp").as_int();

	floaterinfo.alive = f_enem.child("alive").attribute("value").as_bool();

	floaterinfo.rect.w = f_enem.child("size").attribute("w").as_int();
	floaterinfo.rect.h = f_enem.child("size").attribute("h").as_int();

	//roller
	pugi::xml_node r_enem = config.child("enemies").child("enemyground");

	rollerinfo.speed.x = r_enem.child("speed").attribute("x").as_float();
	rollerinfo.speed.y = r_enem.child("speed").attribute("y").as_float();
			  
	rollerinfo.vision_range = r_enem.child("range").attribute("value").as_int();
			  
	rollerinfo.health = r_enem.child("health").attribute("hp").as_int();
			  
	rollerinfo.alive = r_enem.child("alive").attribute("value").as_bool();

	rollerinfo.rect.w = f_enem.child("size").attribute("w").as_int();
	rollerinfo.rect.h = f_enem.child("size").attribute("h").as_int();

	//reading animations
	//floater Idle
	SDL_Rect r;
	float node_speed = -1;

	pugi::xml_node n = config.child("enemyanimations").child("FloatIdle");
	for (n; n; n = n.next_sibling("FloatIdle"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		floaterinfo.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	floaterinfo.idle.speed = (node_speed <= 0) ? floaterinfo.def_anim_speed_enem : node_speed;

	//floater Follow
	n = config.child("enemyanimations").child("FloatFollow");
	for (n; n; n = n.next_sibling("FloatFollow"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		floaterinfo.follow.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	floaterinfo.follow.speed = (node_speed <= 0) ? floaterinfo.def_anim_speed_enem : node_speed;

	//floater die
	//...

	return true;
}

bool j1EntityManager::Start()
{
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

	return true;
}

bool j1EntityManager::CleanUp()
{
	bool ret = false;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->CleanUp();
	}

	return true;
}

bool j1EntityManager::Save(pugi::xml_node & node) const
{
	bool ret = false;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->Save(node, &entities);
	}

	return ret;
}

bool j1EntityManager::Load(pugi::xml_node & node)
{
	p2List_item<Entity*>* item = entities.start;
	pugi::xml_node floater = node.child("enemyair");
	pugi::xml_node roller = node.child("enemyground");
	pugi::xml_node e_static = node.child("enemystatic");
	pugi::xml_node player = node.child("player");

	while (item != nullptr)
	{
		if (item->data->type == entityType::FLOATER)
		{
			item->data->Load(floater, &entities);
		}
		if (item->data->type == entityType::ROLLER)
		{
			item->data->Load(roller, &entities);
		}
		if (item->data->type == entityType::STATIC)
		{
			item->data->Load(e_static, &entities);
		}
		if (item->data->type == entityType::PLAYER)
		{
			item->data->Load(player, &entities);
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
		if (item->data->type == entityType::PLAYER)
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

Entity * j1EntityManager::CreateEntity(entityType type, iPoint pos)
{
	//falta crear classes especifiques
	static_assert(entityType::NO_TYPE == 4, "NO TYPE TO CREATE, CODE NEEDS UPDATE");

	Entity* entity = nullptr;
	switch (type)
	{
	case entityType::FLOATER:

		entity = new Floater(pos);
		break;
		
	//case Entity::entityType::ROLLER:
	//	
	//	entity = new Roller(pos.x, pos.y);
	//	break;
	//	
	//case Entity::entityType::STATIC:
	//	
	//	entity = new Static(pos.x, pos.y);
	//	break;
	//	
	//case Entity::entityType::PLAYER:
	//
	//	entity = new Player(pos.x, pos.y);
	//	break;
	}
	entities.add(entity);

	return entity;
}

bool j1EntityManager::DeleteEntity(Entity * entity)
{
	return entity->CleanUp();
}
