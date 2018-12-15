#include "j1Render.h"
#include "j1Collision.h"
#include "Animation.h"
#include "Path.h"
#include "j1Textures.h"
#include "Floater.h"
#include "Roller.h"
#include "Player.h"
#include "j1Gui.h"
#include "j1EntityManager.h"
#include "j1Audio.h"

#include "SDL_mixer\include\SDL_mixer.h"

j1EntityManager::j1EntityManager()
{
	name.create("entitymanager");
	is_started = false;

	green_counter = blue_counter = score = aux_score = 0;
	score_powUp = 100;	
}

j1EntityManager::~j1EntityManager()
{

}

bool j1EntityManager::Awake(pugi::xml_node & config)
{
	//reading all info
	current_time = config.child("timer").attribute("currenttime").as_float();

	enemyPath = (config.child("enemypath").attribute("path").as_string());
	playerPath = (config.child("playerpath").attribute("path").as_string());

	update_cycle = config.child("enemies").child("updatems").attribute("time").as_float();

	// reading enemies attributes
	//floater
	pugi::xml_node f_enem = config.child("enemies").child("enemyair");

	floaterinfo.type = entityType::FLOATER;
	floaterinfo.rect.w = f_enem.child("size").attribute("w").as_uint();
	floaterinfo.rect.h = f_enem.child("size").attribute("h").as_uint();

	floaterinfo.speed.x = f_enem.child("speed").attribute("x").as_int();
	floaterinfo.speed.y = f_enem.child("speed").attribute("y").as_int();

	floaterinfo.vision_range = f_enem.child("range").attribute("value").as_int();

	floaterinfo.alive = f_enem.child("alive").attribute("value").as_bool();

	//roller
	pugi::xml_node r_enem = config.child("enemies").child("enemyground");

	rollerinfo.type = entityType::ROLLER;
	rollerinfo.rect.w = f_enem.child("size").attribute("w").as_uint();
	rollerinfo.rect.h = f_enem.child("size").attribute("h").as_uint();

	rollerinfo.speed.x = r_enem.child("speed").attribute("x").as_int();
	rollerinfo.speed.y = r_enem.child("speed").attribute("y").as_int();
			  
	rollerinfo.vision_range = r_enem.child("range").attribute("value").as_int();
			  
	rollerinfo.alive = r_enem.child("alive").attribute("value").as_bool();

	//reading animations

	floaterinfo.def_anim_speed = config.child("enemies").child("enemyanimations").child("speed").attribute("defaultAnimationSpeed").as_float();
	rollerinfo.def_anim_speed = floaterinfo.def_anim_speed;

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
	floaterinfo.idle.speed = (node_speed <= 0) ? floaterinfo.def_anim_speed : node_speed;

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
	floaterinfo.follow.speed = (node_speed <= 0) ? floaterinfo.def_anim_speed : node_speed;

	//floater die
	//......


	//ROLLER IDLE

	n = config.child("enemies").child("enemyanimations").child("Spikey");
	for (n; n; n = n.next_sibling("Spikey"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		rollerinfo.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	rollerinfo.idle.speed = (node_speed <= 0) ? rollerinfo.def_anim_speed : node_speed;

	//Loading Player

	//BROFILER_CATEGORY("Player->Awake", Profiler::Color::BlueViolet)
	pugi::xml_node player_node = config.child("player");
	
	playerinfo.type = entityType::PLAYER;
	playerinfo.rect.w = player_node.child("rect").attribute("width").as_uint();
	playerinfo.rect.h = player_node.child("rect").attribute("height").as_uint();

	playerinfo.speed.x = player_node.child("speed").attribute("scrollspeed").as_int();
	playerinfo.speed.y = player_node.child("speed").attribute("jumpspeed").	 as_int();
	playerinfo.smash_speed = player_node.child("speed").attribute("smashspeed").as_int();
	playerinfo.max_speed = player_node.child("speed").attribute("maxspeed").as_float();


	playerinfo.gravity = player_node.child("gravity").attribute("value").as_float();
	playerinfo.god_mode = player_node.child("godmode").attribute("value").as_bool();

	playerinfo.lifes = player_node.child("lifes").attribute("lifes").as_int();
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

	//SMASH
	n = player_node.child("playeranimations").child("smash");
	for (n; n; n = n.next_sibling("smash"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		playerinfo.smash.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	playerinfo.smash.speed = (node_speed <= 0) ? playerinfo.def_anim_speed : node_speed;

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
	playerinfo.god_speed = player_node.child("godmode").attribute("speed").as_float();

	// READING COINS

	otherPath.create(config.child("other").child("path").child_value());
	// Green diamond
	n = config.child("other").child("green_diamond");
	for (n; n; n = n.next_sibling("green_diamond"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		green_diamond.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float(-1);
	green_diamond.idle.speed = (node_speed <= 0) ? green_diamond.def_anim_speed : node_speed;
	green_diamond.idle.loop = true;
	//Defining the entity
	green_diamond.type		= entityType::COIN;
	green_diamond.coin_type = coinType::GREEN_DIAMOND;
	green_diamond.points	= 50;
	green_diamond.alive		= true;
	green_diamond.rect		= { 0,0,r.w,r.h };
	green_diamond.speed		= { 0,0 };
	green_diamond.current_animation = &green_diamond.idle;
	
		
	// Blue diamond
	n = config.child("other").child("blue_diamond");
	for (n; n; n = n.next_sibling("blue_diamond"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		blue_diamond.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float(-1);
	blue_diamond.idle.speed = (node_speed <= 0) ? blue_diamond.def_anim_speed : node_speed;
	blue_diamond.idle.loop = true;
	//Defining the entity
	blue_diamond.type		= entityType::COIN;
	blue_diamond.coin_type	= coinType::BLUE_DIAMOND;
	blue_diamond.points		= 100;
	blue_diamond.alive		= true;
	blue_diamond.rect		= { 0,0,r.w,r.h };
	blue_diamond.speed		= { 0,0 };
	blue_diamond.current_animation = &blue_diamond.idle;

	// Heart life
	n = config.child("other").child("heart");
	for (n; n; n = n.next_sibling("heart"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		heart.idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float(-1);
	heart.idle.speed = (node_speed <= 0) ? heart.def_anim_speed : node_speed;
	heart.idle.loop = true;
	//Defining the entity
	heart.type		= entityType::COIN;
	heart.coin_type = coinType::HEART;
	heart.alive		= true;
	heart.rect		= { 0,0,r.w,r.h };;
	heart.speed		= { 0,0 };
	heart.current_animation = &heart.idle;

	return true;
}

bool j1EntityManager::Start()
{
	BROFILER_CATEGORY("Entities->Start", Profiler::Color::BlueViolet);

	//Loading textures
	playerTex = App->tex->Load(playerPath.GetString());
	enemyTex = App->tex->Load(enemyPath.GetString());
	otherTex = App->tex->Load(otherPath.GetString());

	CreateEntities(playerinfo.lifes, { -1,-1 });


	return true;
}

void j1EntityManager::CreateEntities(int player_lifes, iPoint player_pos)
{
	LOG("CREATING ENTITIES, COUNT: %d", entities.count());
	Entity* e = nullptr;
	
	//Creating all entities
	for (int i = 0; i<App->collision->active; i++)
	{
		if (App->collision->colliders[i] == nullptr) continue;

		iPoint pos = { App->collision->colliders[i]->rect.x , App->collision->colliders[i]->rect.y };

		if (App->collision->colliders[i]->type == COLLIDER_SPAWN_COIN)
		{
			if (App->collision->colliders[i]->rect.w > App->collision->colliders[i]->rect.h)
			{
				e = CreateEntity(COIN, pos, GREEN_DIAMOND);

			}
			else if (App->collision->colliders[i]->rect.w < App->collision->colliders[i]->rect.h)
			{
				e = CreateEntity(COIN, pos, BLUE_DIAMOND);
			}
			else {
				e = CreateEntity(COIN, pos, HEART);
			}
			e->collider = App->collision->AddCollider(e->rect, COLLIDER_COIN);

		}

		else if (App->collision->colliders[i]->type == COLLIDER_SPAWN)
		{
			if (App->collision->colliders[i]->rect.w > App->collision->colliders[i]->rect.h)
			{
				e = CreateEntity(FLOATER, pos);
			}
			else if (App->collision->colliders[i]->rect.w < App->collision->colliders[i]->rect.h)
			{
				e = CreateEntity(ROLLER, pos);
			}
			e->collider = App->collision->AddCollider(e->rect, COLLIDER_DEATH);
		}
		else if (App->collision->colliders[i]->type == COLLIDER_START)
		{
			player_ref = (Player*)CreateEntity(PLAYER, pos);
			player_ref->collider = App->collision->AddCollider(player_ref->rect, COLLIDER_PLAYER, this);
			player_ref->lifes = player_lifes;
			if (player_pos.x != -1) {
				player_ref->position = player_pos;
			}
		}
	}
	LOG("CREATED ENTITIES, COUNT: %d ACTIVE: %d", entities.count(),App->collision->active);
}


bool j1EntityManager::PreUpdate()
{
	BROFILER_CATEGORY("Entities->PreUpdate", Profiler::Color::BlueViolet);

	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type != PLAYER)
		{
			if (!item->data->alive && !item->data->collider->to_delete) 
			{
				item->data->collider->to_delete = true;
				entities.del(item);
			}
		}
	}

	if (player_ref->alive)
		player_ref->PreUpdate();
	else if (!is_started && !App->game_over)
	{
		LOG("IS STARTED == FALSE");
		timer_death.Start();
		is_started = true;
		App->gui->last_death->position = player_ref->position;
		App->gui->last_death->SetInvisible();
		player_ref->current_animation = &player_ref->death;
		App->audio->ToggleMusicPause();
		App->audio->PlayFx(die);
	}

	return true;
}

bool j1EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("Entities->Update", Profiler::Color::BlueViolet)
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
	BROFILER_CATEGORY("Entities->PostUpdate", Profiler::Color::BlueViolet)
	bool ret = false;
	
	if (!player_ref->alive && player_ref->lifes == 0)
	{
		App->gui->game_over->SetVisible();
	}

	if (!player_ref->alive && timer_death.ReadSec() >= 1.5f && !App->game_over) {
		
			LOG("DEAD BY POST UPDATE");
			if (player_ref->lifes > 0) {
				player_ref->lifes--;
				App->RestartLevel(player_ref->lifes);
				LOG("LIFES REMAINING: %d", player_ref->lifes);
			}
			else 
			{
				App->GameOver();
				player_ref->lifes = playerinfo.lifes;				
			}
			player_ref->ResetPlayer();
			is_started = false;
			green_counter = blue_counter = score = 0;

			//timer_death.Start();
		
		return true;
	}

	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		ret = item->data->PostUpdate();
	}

	return true;
}

bool j1EntityManager::CleanUp()
{
	BROFILER_CATEGORY("Entities->CleanUp", Profiler::Color::BlueViolet)
	CleanEntities();

	App->tex->UnLoad(playerTex);
	App->tex->UnLoad(enemyTex);
	App->tex->UnLoad(otherTex);

	
	return true;
}

void j1EntityManager::CleanEntities()
{
	LOG("CLEANING ENTITIES");
	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->collider)
		{
			item->data->collider->to_delete = true;
		}
	}
	entities.clear();
	LOG("ENTITIES CLEANED, COUNT: %d", entities.count());
}


bool j1EntityManager::Restart(int player_lifes, iPoint player_pos)
{
	CleanEntities();
	CreateEntities(player_lifes, player_load_position);
	player_load_position = { -1,-1 };
	return true;
}

void j1EntityManager::UpdateAll(float dt,bool run)
{
	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
			if (!item->data->alive) continue;
			item->data->Update(dt);
	}

	if (run)
	{
		for (item = entities.start; item != nullptr; item = item->next)
		{
			if (!item->data->alive) continue;
			item->data->UpdateLogic(player_ref->position);
		}
	}
}

void j1EntityManager::Draw() {

	p2List_item<Entity*>* item;
	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (!item->data->alive) continue;
		if (item->data->type == entityType::COIN)
		{
			item->data->Draw(otherTex);
		}
		else if (item->data->type != entityType::PLAYER)
		{
			item->data->Draw(enemyTex);
		}
	}
	player_ref->Draw(playerTex);
}

Entity * j1EntityManager::CreateEntity(entityType type, iPoint pos, coinType coin_type)
{
	static_assert(entityType::NO_TYPE == 5, "NO TYPE TO CREATE, CODE NEEDS UPDATE");

	Entity* entity = nullptr;
	switch (type)
	{
	case entityType::FLOATER:
		entity = new Floater(pos, &floaterinfo);
		break;
		
	case entityType::ROLLER:
		entity = new Roller(pos, &rollerinfo);
		break;
		
	case entityType::PLAYER:
		entity = new Player(pos,&playerinfo);
		break;
	case entityType::COIN:
		switch (coin_type)
		{
			case GREEN_DIAMOND:
				entity = new Coin(pos, &green_diamond);
				break;
			case BLUE_DIAMOND:
				entity = new Coin(pos, &blue_diamond);
				break;
			case HEART:
				entity = new Coin(pos, &heart);
				break;
			default:
				LOG("NO COIN TYPE DEFINED, COULD NOT CREATE COIN");
				break;
		}
		break;
	}
	entities.add(entity);

	return entity;
}

void j1EntityManager::OnCollision(Collider * c1, Collider * c2)
{
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_DEATH) {
		if (player_ref->smashing) {
			Entity* e = FindEntityByCollider(c2);
			if (e && e->type != ROLLER) 
			{
				e->alive = false;
				App->audio->PlayFx(dieEnemy);
			}
			else 
			{
				player_ref->alive = false;
			}
		}
		else 
		{
			player_ref->alive = false;
		}
	}
	else if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_COIN) {
		Coin* e = FindCoinByCollider(c2);
		if (e) {
			switch (e->coin_type)
			{
			case GREEN_DIAMOND:
				score += e->points;
				aux_score += e->points;
				green_counter++;
				ChangeUIAnimation(e);
				break;
			case BLUE_DIAMOND:
				score += e->points;
				aux_score += e->points;
				blue_counter++;
				ChangeUIAnimation(e);
				break;
			case HEART:
				player_ref->lifes++;
				ChangeUIAnimation(e);
				break;
			default:
				LOG("NO COIN TYPE DEFINED, COULD NOT CREATE COIN");
				break;
			}
			e->picked = true;
			e->alive = false;
		}
	}
	else if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_END)
	{
		App->NextLevel();
	}
}

Entity* j1EntityManager::FindEntityByCollider(Collider * c)
{
	for (int i = 0; i < entities.count(); ++i) {
		if (entities.At(i)->data->collider == c) {
			return entities.At(i)->data;
		}
	}
	return nullptr;
} 
Coin* j1EntityManager::FindCoinByCollider(Collider * c)
{
	for (int i = 0; i < entities.count(); ++i) {
		if (entities.At(i)->data->collider == c) {
			return (Coin*)entities.At(i)->data;
		}
	}
	return nullptr;
}

void j1EntityManager::ChangeUIAnimation(Coin* c)
{
	if (c->coin_type == GREEN_DIAMOND)
	{
		App->gui->green_ref->current_animation = &App->gui->green_ref->green_shine;
		App->audio->PlayFx(coin);
	}
	if (c->coin_type == BLUE_DIAMOND)
	{
		App->gui->blue_ref->current_animation = &App->gui->blue_ref->blue_shine;
		App->audio->PlayFx(coin);
	}
	if (c->coin_type == HEART)
	{
		App->gui->heart_ref->current_animation = &App->gui->heart_ref->heart_blink;
		App->audio->PlayFx(lifeup);
	}

	App->gui->anim_timer.Start();
}

float j1EntityManager::GetAnimTimer()
{
	return App->gui->anim_timer.ReadSec();
}

bool j1EntityManager::Save(pugi::xml_node & node)
{
	p2List_item<Entity*>* item;

	node.append_attribute("bluespicked") = blue_counter;
	node.append_attribute("greenspicked") = green_counter;
	node.append_attribute("score") = score;

	for (item = entities.start; item != nullptr; item = item->next)
	{


		if (item->data->type == PLAYER)
		{
			pugi::xml_node pl = node.append_child(item->data->name.GetString());

			pl.append_attribute("x") = item->data->position.x;
			pl.append_attribute("y") = item->data->position.y;

			pl.append_attribute("speed_Y") = item->data->speed.y;

			Player* p = (Player*)item->data;
			pl.append_attribute("lives") = p->lifes;
		}
		else if (item->data->type == COIN)
		{
			pugi::xml_node c_node = node.append_child("coins");
			Coin* c = (Coin*)item->data;

			c_node.append_attribute("x") = c->position.x;
			c_node.append_attribute("y") = c->position.y;

			c_node.append_attribute("coin_type") = c->coin_type;

			c_node.append_attribute("picked") = c->picked;
		}
		else
		{
			pugi::xml_node n = node.append_child("enemy");

			n.append_attribute("name") = item->data->name.GetString(); // for debug purposes
			n.append_attribute("type") = item->data->type;
			n.append_attribute("alive") = item->data->alive;

			if (item->data->alive)
			{
				n.append_attribute("x") = item->data->position.x;
				n.append_attribute("y") = item->data->position.y;
			}
		}
	}

	return true;
}

bool j1EntityManager::Load(pugi::xml_node & node)
{
	Restart(player_ref->lifes, { -1,-1 });
	p2List_item<Entity*>* item;

	pugi::xml_node Enode = node.child("enemy");
	pugi::xml_node Cnode = node.child("coins");

	blue_counter = node.attribute("bluespicked").as_int();
	green_counter = node.attribute("greenspicked").as_int();
	score = node.attribute("score").as_int();

	for (item = entities.start; item != nullptr; item = item->next)
	{
		if (item->data->type == PLAYER)
		{
			item->data->position.x = node.child("player").attribute("x").as_int();
			item->data->position.y = node.child("player").attribute("y").as_int();

			item->data->speed.y = node.child("player").attribute("speed_Y").as_int();

			Player* p = (Player*)item->data;
			p->lifes = node.child("player").attribute("lives").as_int();

			item->data = p;
		}
		else if(item->data->type == COIN)
		{
			item->data->position.x = Cnode.attribute("x").as_int();
			item->data->position.y = Cnode.attribute("y").as_int();

			Coin* c = (Coin*)item->data;
			bool rebuild = (c->picked);

			c->coin_type = (coinType)Cnode.attribute("coin_type").as_uint();
			c->picked = Cnode.attribute("picked").as_bool();

			if (c->picked == true)
				c->collider->to_delete = true;

			if (rebuild && !c->picked)
				c->collider = App->collision->AddCollider(c->rect, COLLIDER_COIN, this);

			item->data = c;
			Cnode = Cnode.next_sibling("coins");
		}
		else
		{
			item->data->alive = Enode.attribute("alive").as_bool();

			if (item->data->alive)
			{
				item->data->position.x = Enode.attribute("x").as_int();
				item->data->position.y = Enode.attribute("y").as_int();
				item->data->type = (entityType)Enode.attribute("type").as_uint();
			}
			else
			{
				item->data->collider->to_delete = true;
			}
			Enode = Enode.next_sibling("enemy");
		}
	}
	return true;
}



//DRAFT

/*void j1EntityManager::CheckColliders()
{
	p2List_item<Entity*>* item = entities.start;
	for (item; item != nullptr; item = item->next)
	{
		if (!item->data->alive) 
			continue;
		if (item->data->collider == nullptr) {
			entityType type = item->data->type;
			if (type == FLOATER || type == ROLLER || type == STATIC) {
				item->data->collider = App->collision->AddCollider(item->data->rect, COLLIDER_DEATH);
			}
			else if (type == COIN) {
				item->data->collider = App->collision->AddCollider(item->data->rect, COLLIDER_COIN);
			}
			else if (type == PLAYER) {
				item->data->collider = App->collision->AddCollider(item->data->rect, COLLIDER_PLAYER);
			}
		}
	}
}*/