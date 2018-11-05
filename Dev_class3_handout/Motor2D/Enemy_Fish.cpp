#include "j1App.h"
#include "Enemy_Fish.h"
#include "j1Collision.h"
#include "j1Input.h"


Enemy_Fish::Enemy_Fish(int x, int y) : Enemy(x, y)
{
	path.PushBack({ 1.0f, 0.0f }, 200);

	idle.PushBack({ 123,221,40,40 });
	idle.speed = 0.2f;

	animation = &idle;
	collider = App->collision->AddCollider({ x,y,40,40 }, COLLIDER_TYPE::COLLIDER_DEATH); // callback == App->enemies
	position = iPoint(x, y);
	original_position = iPoint(x, y);
	health = 2;
}

void Enemy_Fish::Shoot() 
{
	//App->particles->AddParticle(App->particles->Eshot1, position.x, position.y + animation->GetCurrentFrame().h / 2, COLLIDER_ENEMY_SHOT);
}

void Enemy_Fish::Move()
{
	position = original_position + path.GetCurrentSpeed(&animation);	
}