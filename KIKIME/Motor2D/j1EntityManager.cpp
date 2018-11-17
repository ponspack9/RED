#include "j1Render.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "j1Textures.h"
#include "Floater.h"
#include "Roller.h"
#include "Player.h"
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

	update_cycle = config.child("enemies").child("updatems").attribute("time").as_float();

	floaterinfo.def_anim_speed_enem = config.child("enemies").child("enemyanimations").child("speed").attribute("defaultAnimationSpeed").as_float();
	rollerinfo.def_anim_speed_enem = floaterinfo.def_anim_speed_enem;

	//floater
	pugi::xml_node f_enem = config.child("enemies").child("enemyair");

	floaterinfo.rect.w = f_enem.child("size").attribute("w").as_uint();
	floaterinfo.rect.h = f_enem.child("size").attribute("h").as_uint();

	floaterinfo.speed.x = f_enem.child("speed").attribute("x").as_int();
	floaterinfo.speed.y = f_enem.child("speed").attribute("y").as_int();

	floaterinfo.vision_range = f_enem.child("range").attribute("value").as_int();

	floaterinfo.health = f_enem.child("health").attribute("hp").as_int();

	floaterinfo.alive = f_enem.child("alive").attribute("value").as_bool();

	//roller
	pugi::xml_node r_enem = config.child("enemies").child("enemyground");

	rollerinfo.rect.w = f_enem.child("size").attribute("w").as_uint();
	rollerinfo.rect.h = f_enem.child("size").attribute("h").as_uint();

	rollerinfo.speed.x = r_enem.child("speed").attribute("x").as_int();
	rollerinfo.speed.y = r_enem.child("speed").attribute("y").as_int();
			  
	rollerinfo.vision_range = r_enem.child("range").attribute("value").as_int();
			  
	rollerinfo.health = r_enem.child("health").attribute("hp").as_int();
			  
	rollerinfo.alive = r_enem.child("alive").attribute("value").as_bool();

	//reading animations
	//floater Idle
	SDL_Rect r;
	float node_speed = -1;

	pugi::xml_node n = config.child("enemies").child("enemyanimations").child("FloatIdle");
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
	n = config.child("enemies").child("enemyanimations").child("FloatFollow");
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
	//......


	//ROLLER IDLE

	n = config.child("enemies").child("enemyanimations").child("Walk");
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

	//Loading Player

	//BROFILER_CATEGORY("Player->Awake", Profiler::Color::BlueViolet)
	pugi::xml_node player_node = config.child("player");

	playerinfo.rect.w = player_node.child("rect").attribute("width").as_uint();
	playerinfo.rect.h = player_node.child("rect").attribute("height").as_uint();

	playerinfo.speed.x = player_node.child("speed").attribute("scrollspeed").as_int();
	playerinfo.speed.y = player_node.child("speed").attribute("jumpspeed").	 as_int();

	playerinfo.gravity = player_node.child("gravity").attribute("value").as_float();

	// Parsing animations ----------------

	playerinfo.def_anim_speed = player_node.child("playeranimations").attribute("defaultAnimationSpeed").as_float();
	// IDLE
	n = player_node.child("playeranimations").child("idle");
	for (n; n; n = n.next_sibling("idle"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.idle.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	playerinfo.rect.w = playerinfo.idle.GetCurrentFrame().w;
	playerinfo.rect.h = playerinfo.idle.GetCurrentFrame().h;

	// WALK
	n = player_node.child("playeranimations").child("walk");
	for (n; n; n = n.next_sibling("walk"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.walk.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.walk.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	// JUMP
	n = player_node.child("playeranimations").child("jump");
	for (n; n; n = n.next_sibling("jump"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.jump.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.jump.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	//DOUBLE JUMP
	n = player_node.child("playeranimations").child("doublejump");
	for (n; n; n = n.next_sibling("doublejump"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.doublejump.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.doublejump.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	//FALL
	n = player_node.child("playeranimations").child("fall");
	for (n; n; n = n.next_sibling("fall"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.fall.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.fall.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	//DEAD
	n = player_node.child("playeranimations").child("death");
	for (n; n; n = n.next_sibling("death"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.death.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.death.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	//GOD
	n = player_node.child("playeranimations").child("god");
	for (n; n; n = n.next_sibling("god"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.god.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.god.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

	// End parsing animations -----------------

	//LOG("%d  %d", player_rect.h, player_rect.w);
	LOG("%d  %d", playerinfo.speed.x, playerinfo.speed.y);
	
	playerinfo.collider_offset = playerinfo.speed.x;
	playerinfo.max_speed_y = playerinfo.speed.y;
	return true;
}

bool j1EntityManager::Start()
{
	BROFILER_CATEGORY("Entities->Start", Profiler::Color::BlueViolet)

	//Loading textures
	playerTex = App->tex->Load(playerPath.GetString());
	enemyTex = App->tex->Load(enemyPath.GetString());

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
		if (App->collision->colliders[i]->type == COLLIDER_START)
		{
			iPoint pos = { App->collision->colliders[i]->rect.x , App->collision->colliders[i]->rect.y };
			player_ref = CreateEntity(PLAYER, pos);

			player_ref->collider_identifier = App->collision->AddCollider(player_ref->rect, COLLIDER_NONE, this);
			player_ref->collider = App->collision->AddCollider(player_ref->rect, COLLIDER_PLAYER, this);

			SDL_Rect temp = player_ref->rect;
			temp.h -= player_ref->speed.x;
			player_ref->collider_ray_right = App->collision->AddCollider(player_ref->rect, COLLIDER_RAY_RIGHT, this);
			player_ref->collider_ray_left = App->collision->AddCollider(player_ref->rect, COLLIDER_RAY_LEFT, this);
			temp = player_ref->rect;
			temp.w -= player_ref->speed.y;
			player_ref->collider_ray_down = App->collision->AddCollider(player_ref->rect, COLLIDER_RAY_DOWN, this);
			player_ref->collider_ray_up = App->collision->AddCollider(player_ref->rect, COLLIDER_RAY_UP, this);

		}
	}
	return true;
}

bool j1EntityManager::PreUpdate()
{
	bool ret = true;
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == PLAYER)
			ret = item->data->PreUpdate();
	}

	return ret;
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
	p2List_item<Entity*>* item;

	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->collider != nullptr)
		{
			item->data->collider->to_delete = true;
		}
	}
	App->tex->UnLoad(playerTex);
	App->tex->UnLoad(enemyTex);

	entities.clear();
	
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
			pugi::xml_node pl = node.append_child(item->data->name.GetString());

			pl.append_attribute("x") = item->data->position.x;
			pl.append_attribute("y") = item->data->position.y;
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


	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == PLAYER)
		{
			item->data->position.x = node.child("player").attribute("x").as_float();
			item->data->position.y = node.child("player").attribute("y").as_float();
		}
		else
		{
			node = node.child("enemy");

			item->data->alive = node.attribute("alive").as_bool();
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
	}
	return true;
}

bool j1EntityManager::Restart()
{
	bool ret = false;

	ret = CleanUp();
	ret = Start();

	return ret;
}

void j1EntityManager::UpdateAll(float dt,bool run)
{
	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == entityType::PLAYER)
		{
			item->data->Update(dt);
			//item->data->Draw(playerTex);
		}
		else
		{
			item->data->Update(dt);
			item->data->Draw(enemyTex);
		}
	}
	if (run)
	{
		for (item = entities.start; item != nullptr; item = item->next)
		{
			item->data->UpdateLogic(player_ref->position);
		}
	}
}

Entity * j1EntityManager::CreateEntity(entityType type, iPoint pos)
{
	//falta crear classes especifiques
	static_assert(entityType::NO_TYPE == 4, "NO TYPE TO CREATE, CODE NEEDS UPDATE");

	Entity* entity = nullptr;
	Entity* e = nullptr;
	Player* pl = nullptr;
	switch (type)
	{
	case entityType::FLOATER:
		e = &floaterinfo;
		entity = new Floater(pos, e,type);
		break;
		
	case entityType::ROLLER:
		e = &rollerinfo;
		entity = new Roller(pos, e, type);
		break;
		
	case entityType::PLAYER:
		pl = &playerinfo;
		entity = new Player(pos, pl, playerTex, type);
		n++;
		break;
	}
	//case entityType::STATIC:
	//	
	//	entity = new Static(pos.x, pos.y);
	//	break;
	//	
	entities.add(entity);

	return entity;
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	//*collider_identifier = c2;

	if (c1->type == COLLIDER_RAY_RIGHT && c2->type == COLLIDER_FLOOR) {
		player_ref->can_move_right = false;
		player_ref->horizontal_collided = true;
		//LOG("COLLIDED_RAY_RIGHT");
	}
	if (c1->type == COLLIDER_RAY_LEFT && c2->type == COLLIDER_FLOOR) {
		player_ref->can_move_left = false;
		player_ref->horizontal_collided = true;
		//LOG("COLLIDED_RAY_LEFT");

	}
	if (c1->type == COLLIDER_RAY_UP && c2->type == COLLIDER_FLOOR) {
		player_ref->can_move_up = false;
		player_ref->can_move_down = true;
		player_ref->on_floor = false;
		player_ref->vertical_collided = true;
		//LOG("COLLIDED_RAY_UP");

	}
	if (c1->type == COLLIDER_RAY_DOWN && c2->type == COLLIDER_FLOOR) {
		player_ref->can_move_down = false;
		player_ref->on_floor = true;
		player_ref->is_falling = false;
		player_ref->djump = false;
		player_ref->aux_djump = false;
		player_ref->vertical_collided = true;
		//LOG("COLLIDED_RAY_DOWN");

	}
	if (!player_ref->can_move_down && !player_ref->can_move_left && !player_ref->can_move_right) {
		player_ref->can_move_left = true;
		player_ref->can_move_right = true;
		player_ref->is_falling = false;
		player_ref->on_floor = true;
		//LOG("SPECIAL CASE");
	}

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_END && !player_ref->level_finished) {
		player_ref->level_finished = true;
	}
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_DEATH) {
		player_ref->dead = true;
	}

}
