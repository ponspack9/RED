#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Entity.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "Enemy_Air.h"
#include "p2Log.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"


#define SPAWN_MARGIN 10

Entity::Entity()
{
	
}

Entity::Entity(entityType type)
{

}

// Destructor
Entity::~Entity()
{

}

bool Entity::Awake(pugi::xml_node & config)
{
	pugi::xml_node enem_node = config; 

	//enemy rect
	enemy_air_rect.w = enem_node.child("rect").attribute("width").as_int();
	enemy_air_rect.h = enem_node.child("rect").attribute("height").as_int();

	//enemy speed
	entities[0]->speed.x = enem_node.child("speed").attribute("movingVel").as_int();
	entities[0]->speed.y = entities[0]->speed.x;

	entities[0]->def_anim_speed_enem = enem_node.child("speed").attribute("defaultAnimationSpeed").as_float();

	pugi::xml_node enemy_anims = enem_node.child("TextureAtlas");
	texture_path = (enemy_anims.attribute("imagePath").as_string());

	SDL_Rect r;
	float node_speed = -1;

	//Enemy Idle
	pugi::xml_node n = enemy_anims.child("FloatIdle");
	for (n; n; n = n.next_sibling("FloatIdle"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		entities[0]->idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	entities[0]->idle.speed = (node_speed <= 0) ? entities[0]->def_anim_speed_enem : node_speed;

	enemy_air_rect.w = entities[0]->idle.GetCurrentFrame().w;
	enemy_air_rect.h = entities[0]->idle.GetCurrentFrame().h;

	//Enemy Follow
	n = enemy_anims.child("FloatFollow");
	for (n; n; n = n.next_sibling("FloatFollow"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		entities[0]->follow.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	entities[0]->follow.speed = (node_speed <= 0) ? entities[0]->def_anim_speed_enem : node_speed;

	enemy_air_rect.w = entities[0]->follow.GetCurrentFrame().w;
	enemy_air_rect.h = entities[0]->follow.GetCurrentFrame().h;

	return true;
}

bool Entity::Start()
{
	// Create a prototype for each enemy available so we can copy them around
	sprites = App->tex->Load(texture_path.GetString());
	//Explosion = App->audio->LoadFx("assets/audio/Explosion.wav");

	return true;
}

bool Entity::PreUpdate()
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
bool Entity::Update(float dt)
{

	return true;
}

bool Entity::PostUpdate()
{
	// check camera position to decide what to spawn
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(entities[i] != nullptr)
		{
			delete entities[i];
			entities[i] = nullptr;
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
bool Entity::CleanUp()
{
	LOG("Freeing all enemies");

	App->tex->UnLoad(sprites);

	for(uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if(entities[i] != nullptr)
		{
			delete entities[i];
			entities[i] = nullptr;
		}
	}

	return true;
}

bool Entity::AddEnemy(ENEMY_TYPES type, int x, int y)
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

void Entity::SpawnEnemy(const ENEMY_TYPES& type)
{
	// find room for the new enemy
	uint i = 0;
	for(; enemies[i] != nullptr && i < MAX_ENEMIES; ++i);

	if(i != MAX_ENEMIES)
	{
		switch(type.type)
		{
			case ENEMY_TYPES::ENEMY:
			enemies[i] = new Entity(type);
			LOG("You want to spawn a ENEMY_AIR");
			break;

			
		}
	}
}

void Entity::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ENEMIES; ++i)
	{
		if (entities[i] != nullptr)
		{
			entities[i]->first_iteration = true;
		}
		/*if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
		{
			LOG("Collision with enemy with index: %d", i);			
		}*/
	}

}

const Collider * Entity::GetCollider() const
{
	return collider;
}