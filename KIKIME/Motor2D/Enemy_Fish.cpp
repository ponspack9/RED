#include "j1App.h"
#include "Enemy_Fish.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"

Enemy_Fish::Enemy_Fish(int x, int y) : Enemy(x, y)
{
	//path.PushBack({ 0.0f, 0.0f }, 1);

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
	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(App->scene->debug_tex, pos.x, pos.y);
		//LOG("[%d,%d]", path->At(i)->x, path->At(i)->y);
	}
	if (path->Count() > 0) {
		//LOG("Path[%d,%d]", path->At(1)->x, path->At(1)->y);
		//LOG("Poss[%d,%d]", p.x,p.y);
		position += iPoint (path->At(1)->x, path->At(1)->y) - p;
	}
	path->Clear();
	//const p2DynArray arraay =  App->pathfinding->GetLastPath()->Pop(velocity);
	//position = original_position + path.GetCurrentSpeed(&animation);	
}