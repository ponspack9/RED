#include "j1App.h"
#include "Enemy_Ground.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"
#include "Enemy.h"
#include "j1Enemies.h"

Enemy_Ground::Enemy_Ground(int x, int y) : Enemy(x, y)
{
	//current_animation = &App->enemies->prototypes[(int)Enemy_Ground].idle;
	//collider = App->collision->AddCollider(App->enemies->Enemy_Ground_rect, COLLIDER_TYPE::COLLIDER_DEATH,App->enemies); 
	idle.PushBack({ 123,221,40,40 });
	idle.speed = 0.2f;

	current_animation = &idle;
	collider = App->collision->AddCollider({ x,y,40,40 }, COLLIDER_TYPE::COLLIDER_DEATH, App->enemies);
	position = iPoint(x, y);
	health = 2;
}

void Enemy_Ground::Shoot() 
{
	//App->particles->AddParticle(App->particles->Eshot1, position.x, position.y + animation->GetCurrentFrame().h / 2, COLLIDER_ENEMY_SHOT);
}

void Enemy_Ground::Move()
{
	//Origin
	iPoint p = App->render->ScreenToWorld(position.x + App->render->camera.x, position.y + App->render->camera.y);
	p = App->map->WorldToMap(p.x, p.y);
	//Destination
	iPoint a = App->render->ScreenToWorld(App->player->position.x + App->render->camera.x, App->player->position.y + App->render->camera.y);
	a = App->map->WorldToMap(a.x, a.y);

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
	}

	//Move the enemy towards player
	if (path->Count() > 1 && App->input->GetKey(SDL_SCANCODE_B) == KEY_REPEAT/* && path->Count() < can_see*/) {

		if (desired_position == position) 	speed = iPoint(path->At(1)->x, path->At(1)->y) - p;

		desired_position = App->map->MapToWorld(p.x, p.y);

		if (speed.x > 0 && speed.y == 0) {
			desired_position = App->map->MapToWorld(p.x +1, p.y);
		}
		else if (speed.y > 0 && speed.x == 0) {
			desired_position = App->map->MapToWorld(p.x, p.y +1);
		}
		//Happens only when diagonal movemnt implemented
		/*if (speed.y > 0 && speed.x > 0) {
			desired_position = App->map->MapToWorld(p.x+1, p.y + 1);
		}*/
		position += speed;
	}
	path->Clear();
}