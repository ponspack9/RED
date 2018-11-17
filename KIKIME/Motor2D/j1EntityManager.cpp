#include "j1Render.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "j1Textures.h"
#include "Floater.h"
#include "Roller.h"
#include "Static.h"
#include "j1EntityManager.h"

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

	pugi::xml_node n = config.child("enemies").child("enemyanimations").child("TextureAtlas").child("FloatIdle");
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
	n = config.child("enemies").child("enemyanimations").child("TextureAtlas").child("FloatFollow");
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
	//ROLLER IDLE

	n = config.child("enemies").child("enemyanimations").child("TextureAtlas").child("Walk");
	for (n; n; n = n.next_sibling("Walk"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		rollerinfo.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	rollerinfo.idle.speed = (node_speed <= 0) ? rollerinfo.def_anim_speed_enem : node_speed;

	return true;
}

bool j1EntityManager::Start()
{
	//Loading textures
	enemyTex = App->tex->Load(enemyPath.GetString());
	playerTex = App->tex->Load(playerPath.GetString());

	//Creating all entities
	for (int i = 0; App->collision->colliders[i] != NULL; i++)
	{
		if (App->collision->colliders[i]->type == COLLIDER_SPAWN)
		{
			if (App->collision->colliders[i]->rect.w > App->collision->colliders[i]->rect.h)
			{
				iPoint pos = { App->collision->colliders[i]->rect.x , App->collision->colliders[i]->rect.y };
				CreateEntity(FLOATER, pos);
			}
			if (App->collision->colliders[i]->rect.w < App->collision->colliders[i]->rect.h)
			{
				iPoint pos = { App->collision->colliders[i]->rect.x , App->collision->colliders[i]->rect.y + (App->collision->colliders[i]->rect.h / 2) };
				CreateEntity(ROLLER, pos);
			}
		}
	}
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

bool j1EntityManager::Save(pugi::xml_node & node)
{
	p2List_item<Entity*>* item;
	LOG("saving enemiesHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");


	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == PLAYER)
		{
			//////////////////////////////////
		}
		else
		{
			//pugi::xml_node n = node.append_child(item->data->name.GetString());
			pugi::xml_node n = node.append_child("enemy");

			n.append_attribute("type") = item->data->name.GetString();
			n.append_attribute("alive") = item->data->alive;
			n.append_attribute("hp") = item->data->health;

			if (item->data->alive)
			{
				n.append_attribute("x") = item->data->position.x;
				n.append_attribute("y") = item->data->position.y;
			}
		}
		LOG("SAVE POS: %d  ,  %d", item->data->position.x, item->data->position.y);
		LOG("SAVE HP:  %d", item->data->health);
	}

	return true;
}

bool j1EntityManager::Load(pugi::xml_node & node)
{
	p2List_item<Entity*>* item;
	LOG("loading enemiesEEEEEEEEEEEEEEEEEEEEEEE");

	node = node.child("enemy");

	for (item = entities.start; item != nullptr; item = item->next)
	{
		item->data->alive =	 node.attribute("alive").as_bool();
		item->data->health = node.attribute("hp").as_int();

		if (item->data->alive)
		{
			item->data->position.x = node.attribute("x").as_int();
			item->data->position.y = node.attribute("y").as_int();
		}
		LOG("LOAD POS: %d  ,  %d", item->data->position.x, item->data->position.y);
		LOG("LOAD HP:  %d", item->data->health);

		node = node.next_sibling("enemy");
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
		else
		{
			item->data->Update(dt);
		}
	}


	if (run)
	{
		for (item = entities.start; item != nullptr; item = item->next)
		{
			item->data->UpdateLogic();
		}
	}
}

Entity * j1EntityManager::CreateEntity(entityType type, iPoint pos)
{
	//falta crear classes especifiques
	static_assert(entityType::NO_TYPE == 4, "NO TYPE TO CREATE, CODE NEEDS UPDATE");

	Entity* entity = nullptr;
	Entity* e = nullptr;
	switch (type)
	{
	case entityType::FLOATER:
		e = &floaterinfo;
		entity = new Floater(pos, e, enemyTex);
		break;
		
	case entityType::ROLLER:
		e = &rollerinfo;
		entity = new Roller(pos, e, enemyTex);
		break;
		
	//case entityType::STATIC:
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
