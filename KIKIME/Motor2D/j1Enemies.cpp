#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "Enemy_Air.h"
#include "Enemy_Ground.h"
#include "p2Log.h"
#include "j1Pathfinding.h"


#define SPAWN_MARGIN 10

j1Enemies::j1Enemies()
{
	name.create("enemies");

	for(uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;
}

// Destructor
j1Enemies::~j1Enemies()
{
}

bool j1Enemies::Awake(pugi::xml_node & config)
{
	pugi::xml_node enem_node = config; 

	////enemy rect
	//enemy_air_rect.w = enem_node.child("rect").attribute("width").as_int();
	//enemy_air_rect.h = enem_node.child("rect").attribute("height").as_int();

	////enemy speed
	//prototypes[(int)ENEMY_AIR]->speed.x = enem_node.child("speed").attribute("movingVel").as_int();
	//prototypes[(int)ENEMY_AIR]->speed.y = prototypes[(int)ENEMY_AIR]->speed.x;

	//prototypes[(int)ENEMY_AIR]->def_anim_speed_enem = enem_node.child("speed").attribute("defaultAnimationSpeed").as_float();

	//pugi::xml_node enemy_anims = enem_node.child("TextureAtlas");
	//texture_path = (enemy_anims.attribute("imagePath").as_string());

	//SDL_Rect r;
	//float node_speed = -1;

	////Enemy Idle
	//pugi::xml_node n = enemy_anims.child("FloatIdle");
	//for (n; n; n = n.next_sibling("FloatIdle"))
	//{
	//	r.x = n.attribute("x").as_int();
	//	r.y = n.attribute("y").as_int();
	//	r.w = n.attribute("width").as_int();
	//	r.h = n.attribute("height").as_int();
	//	prototypes[(int)ENEMY_AIR]->idle.PushBack(r);
	//}
	//node_speed = n.attribute("speed").as_float();
	//prototypes[(int)ENEMY_AIR]->idle.speed = (node_speed <= 0) ? prototypes[(int)ENEMY_AIR]->def_anim_speed_enem : node_speed;

	//enemy_air_rect.w = prototypes[(int)ENEMY_AIR]->idle.GetCurrentFrame().w;
	//enemy_air_rect.h = prototypes[(int)ENEMY_AIR]->idle.GetCurrentFrame().h;

	////Enemy Follow
	//n = enemy_anims.child("FloatFollow");
	//for (n; n; n = n.next_sibling("FloatFollow"))
	//{
	//	r.x = n.attribute("x").as_int();
	//	r.y = n.attribute("y").as_int();
	//	r.w = n.attribute("width").as_int();
	//	r.h = n.attribute("height").as_int();
	//	prototypes[(int)ENEMY_AIR]->follow.PushBack(r);
	//}
	//node_speed = n.attribute("speed").as_float();
	//prototypes[(int)ENEMY_AIR]->follow.speed = (node_speed <= 0) ? prototypes[(int)ENEMY_AIR]->def_anim_speed_enem : node_speed;

	//enemy_air_rect.w = prototypes[(int)ENEMY_AIR]->follow.GetCurrentFrame().w;
	//enemy_air_rect.h = prototypes[(int)ENEMY_AIR]->follow.GetCurrentFrame().h;

	return true;
}

bool j1Enemies::Start()
{
	// Create a prototype for each enemy available so we can copy them around
	//sprites = App->tex->Load(texture_path.GetString());
	sprites = App->player->graphics;
	//Explosion = App->audio->LoadFx("assets/audio/Explosion.wav");

	return true;
}

bool j1Enemies::PreUpdate()
{
	// check camera position to decide what to spawn
	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(queue[i].type != ENEMY_TYPES::NO_TYPE)
		{
			if(queue[i].x < abs(App->render->camera.x) + App->render->viewport.w)
			{
				SpawnEnemy(queue[i]);
				queue[i].type = ENEMY_TYPES::NO_TYPE;
			}
		}
		else {
			break;
		}
	}


	return true;
}

// Called before render is available
bool j1Enemies::Update(float dt)
{
	for(uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) {
			enemies[i]->Move();


		}

	for(uint i = 0; i < MAX_ENEMIES; ++i)
		if (enemies[i] != nullptr) {
			enemies[i]->Draw(sprites);
			
		}

	return true;
}

bool j1Enemies::PostUpdate()
{
	// check camera position to decide what to spawn
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
			/*("Enemy found at index: %d", i);

			if(enemies[i]->position.x * App->win->GetScale() < (App->render->camera.w) - SPAWN_MARGIN)
			{
				LOG("DeSpawning enemy at %d", enemies[i]->position.x * App->win->GetScale());
				
			}*/
	}
	return true;
}

// Called before quitting
bool j1Enemies::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(sprites);

	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}

	return true;
}

bool j1Enemies::AddEnemy(ENEMY_TYPES type, int x, int y)
{
	bool ret = false;

	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(queue[i].type == ENEMY_TYPES::NO_TYPE)
		{
			LOG("To Spawn at %d,%d", x, y);
			queue[i].type = type;
			queue[i].x = x;
			queue[i].y = y;
			ret = true;
			break;
		}
	}

	return ret;
}

void j1Enemies::SpawnEnemy(const EnemyInfo& info)
{
	// find room for the new enemy
	uint i = 0;
	for(; enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	if(i != MAX_ENEMIES)
	{
		switch(info.type)
		{
			case ENEMY_TYPES::ENEMY_AIR:
				enemies[i] = new Enemy_Air(info.x, info.y);
				LOG("You want to spawn a ENEMY_AIR");
			break;
			case ENEMY_TYPES::ENEMY_GROUND:
				enemies[i] = new Enemy_Ground(info.x, info.y);
				LOG("You want to spawn a ENEMY_Ground");
			break;

			
		}
	}
}

void j1Enemies::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (enemies[i] != nullptr)
		{
			enemies[i]->first_iteration = true;
		}
		/*if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			LOG("Collision with enemy with index: %d", i);			
		}*/
	}

}