#include "j1App.h"
#include "Enemy_Fish.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Enemies.h"

Enemy_Fish::Enemy_Fish(int x, int y) : Enemy(x, y)
{
	//path.PushBack({ 0.0f, 0.0f }, 1);

	idle.PushBack({ 123,221,40,40 });
	idle.speed = 0.2f;

	animation = &idle;
	collider = App->collision->AddCollider({ x,y,40,40 }, COLLIDER_TYPE::COLLIDER_DEATH,App->enemies); // callback == App->enemies
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
	iPoint velocity;
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//p = App->map->MapToWorld(p.x, p.y);  
	//LOG("Origin [%d,%d]", p.x, p.y);

	iPoint a = App->render->ScreenToWorld(App->player->position.x + App->render->camera.x, App->player->position.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);
	//a = App->map->MapToWorld(a.x, a.y);
	//LOG("Destination [%d,%d]", a.x, a.y);

	App->pathfinding->CreatePath(p, a);
	p2DynArray<iPoint>* path = App->pathfinding->GetLastPathNotConst();
	if (first_iteration) {
		speed = iPoint(path->At(1)->x, path->At(1)->y) - p;
		first_iteration = false;
	}
	//Blit path to screen
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
		//LOG("[%d,%d]", path->At(i)->x, path->At(i)->y);
	}
	//Move the enemy towards player
	if (path->Count() > 1 && App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT/* && path->Count() < can_see*/) {
		LOG("Path[%d,%d]", path->At(1)->x, path->At(1)->y);
		LOG("Poss[%d,%d]", p.x,p.y);

		//iPoint velocity = App->map->MapToWorld(path->At(1)->x, path->At(1)->y) - position;
		if (desired_position == position) 
			speed = iPoint(path->At(1)->x, path->At(1)->y) - p;
		desired_position = App->map->MapToWorld(p.x, p.y);

		if (speed.x > 0 && speed.y == 0) {
			desired_position = App->map->MapToWorld(p.x +1, p.y);
		}
		if (speed.y > 0 && speed.x == 0) {
			desired_position = App->map->MapToWorld(p.x, p.y +1);
		}
		//Happens only when diagonal movemnt implemented
		/*if (speed.y > 0 && speed.x > 0) {
			desired_position = App->map->MapToWorld(p.x+1, p.y + 1);
		}*/
		 
		LOG("Velocity[%d,%d]", speed.x, speed.y);
		
		LOG("Poss[%d,%d]", position.x, position.y);
		LOG("Desired[%d,%d]", desired_position.x, desired_position.y);

		//velocity = last_velocity;
		/*velocity.x *= App->dt;
		velocity.y *= App->dt;*/

		position += speed;
		last_velocity = speed;
	}
	/*if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	{
		int breakpoint = 1;

	}*/
	path->Clear();
	//const p2DynArray arraay =  App->pathfinding->GetLastPath()->Pop(velocity);
	//position = original_position + path.GetCurrentSpeed(&animation);	
}