#include "j1App.h"
#include "Floater.h"
#include "j1Input.h"
#include "j1Pathfinding.h"
#include "j1Player.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1Render.h"
#include "p2Log.h"


Floater::Floater(iPoint pos,Entity* e, SDL_Texture* sprites) : Entity(type)
{
	name.create("floater");
	
	this->sprites = sprites;
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = e->speed;

	idle = e->idle;
	health = e->health;
	alive = e->alive;
	vision_range = e->vision_range;

	current_animation = &idle;
	LOG("Floater Created");
	LOG("pos %d, %d",position.x,position.y);
}

Floater::~Floater()
{

}

bool Floater::Update(float dt)
{
	bool ret = true;
	Move();
	return ret;
}

bool Floater::PostUpdate()
{
	Draw();
	return true;
}

bool Floater::CleanUp()
{
	return true;
}

void Floater::Shoot() 
{
	//App->particles->AddParticle(App->particles->Eshot1, position.x, position.y + animation->GetCurrentFrame().h / 2, COLLIDER_ENEMY_SHOT);
}

bool Floater::UpdateLogic()
{
	return false;
}

void Floater::Draw()
{
	App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0);
}

void Floater::Move()
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