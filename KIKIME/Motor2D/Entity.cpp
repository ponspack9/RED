#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "Entity.h"
#include "j1Textures.h"
#include "j1Window.h"
#include "j1Player.h"
#include "Floater.h"
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

void Entity::Draw(float dt)
{

}

void Entity::OnCollision(Collider* c1, Collider* c2)
{
	//for (uint i = 0; i < MAX_ENEMIES; ++i)
	//{
	//	if (entities[i] != nullptr)
	//	{
	//		entities[i]->first_iteration = true;
	//	}
	//	/*if (enemies[i] != nullptr && enemies[i]->GetCollider() == c1)
	//	{
	//		LOG("Collision with enemy with index: %d", i);			
	//	}*/
	//}

}

const Collider * Entity::GetCollider() const
{
	return collider;
}