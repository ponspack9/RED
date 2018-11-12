#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Enemies.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "Enemy_Fish.h"
#include "p2Log.h"
#include "Enemy.h"
#include "j1Pathfinding.h"


#define SPAWN_MARGIN 10

j1Enemies::j1Enemies()
{
	for(uint i = 0; i < MAX_ENEMIES; ++i)
		enemies[i] = nullptr;
}

// Destructor
j1Enemies::~j1Enemies()
{
}

bool j1Enemies::Start()
{
	// Create a prototype for each enemy available so we can copy them around
	//sprites = App->tex->Load("assets/tengai/Enemy.png");
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
			if(queue[i].x * App->win->GetScale() < App->render->camera.x + (App->render->camera.w * App->win->GetScale()) + SPAWN_MARGIN)
			{
				LOG(" RIGHT Spawning enemy at %d", queue[i].x * App->win->GetScale());
				SpawnEnemy(queue[i]);
				queue[i].type = ENEMY_TYPES::NO_TYPE;
			}
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
			case ENEMY_TYPES::BLACK_NIGGA:
			enemies[i] = new Enemy_Fish(info.x, info.y);
			LOG(" RIGHT You want to spawn a BLACK NIGGA");
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