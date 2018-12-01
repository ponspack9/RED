#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Particles.h"
#include "p2Log.h"

#include <stdlib.h>
#include <stdio.h>
#include "SDL/include/SDL_timer.h"

j1Particles::j1Particles()
{
	name.create("particles");
	
	for(uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
		active[i] = nullptr;
	
}

bool j1Particles::Awake(pugi::xml_node & config)
{
	graphics_path.create(config.child("path").child_value());

	pugi::xml_node n = config.child("diamond");

	float node_speed = -1;
	float def_anim_speed = config.attribute("def_anim_speed").as_float();
	SDL_Rect r;

	for (n; n; n = n.next_sibling("FloatIdle"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		coin.anim.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float(-1);
	coin.anim.speed = (node_speed <= 0) ? def_anim_speed : node_speed;
	coin.anim.loop = true;





	return true;
}

// Load assets
bool j1Particles::Start()
{
	LOG("Loading particles");
	graphics = App->tex->Load(graphics_path.GetString());
	
	return true;
}



// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");
	App->tex->UnLoad(graphics);

	for(uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if(active[i] != nullptr)
		{
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

// Update: draw background
bool j1Particles::Update(float dt)
{
	for(uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		Particle* p = active[i];
		if (p != nullptr) {
			int breakpoint = 0;
		}
		if(p == nullptr)
			break;

		if(p->Update() == false)
		{
			if (p->collider) 	p->collider->to_delete = true;
			delete p;
			active[i] = nullptr;
		}
		else if(SDL_GetTicks() >= p->born)
		{
			App->render->Blit(graphics, p->position.x, p->position.y, &(p->anim.GetCurrentFrame()));
			if(p->fx_played == false)
			{
				p->fx_played = true;
				// Play particle fx here
			}
		}
	}

	return true;
}

Particle* j1Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, Uint32 delay)
{
	Particle* p = nullptr;
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		int a = 0;
		if (active[i] == nullptr)
		{
			p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = x;
			p->position.y = y;
			if (collider_type != COLLIDER_NONE)
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), collider_type, this);
			active[i] = p;
			LOG("PARTICLE CREATED AT [%d,%d]", x, y);
			break;
		}
	}
	
	return p;
}
Particle* j1Particles::AddParticle(const Particle& particle, iPoint pos, COLLIDER_TYPE collider_type, Uint32 delay)
{
	Particle* p = nullptr;

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		
		if (active[i] == nullptr)
		{
			p = new Particle(particle);
			p->born = SDL_GetTicks() + delay;
			p->position.x = pos.x;
			p->position.y = pos.y;
			if (collider_type != COLLIDER_NONE)
				p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), collider_type, this);
			active[i] = p;
			break;
		}
	}
	return p;
}

void j1Particles::Reset() {
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {
		Particle* p = active[i];
		if (p == nullptr)		return;
		if (p->collider) 	p->collider->to_delete = true;
		delete p;
		active[i] = nullptr;
		
	}
}
// -------------------------------------------------------------
void j1Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr && active[i]->collider == c1)
		{
			switch (c2->type) {

			default:
				LOG("COLLIDED WITH ONE PARTICLE WITH INDEX %d", i);
				return;
			}
		}
		
	}
	
}
// ---------------------- PARTICLE ---------------------------------------

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) : 
anim(p.anim), position(p.position), speed(p.speed),
fx(p.fx), born(p.born), life(p.life)
{}

bool Particle::Update()
{
	bool ret = true;

	if (life == 0) {
		//ret = InScreen();
		ret = true;
	}
	else if(life > 0)
	{
		if((SDL_GetTicks() - born) > life)
			ret = false;
	}
	else
		if(anim.Finished())
			ret = false;

	if (collider != nullptr)
		collider->SetPos(position.x, position.y);
	
	return ret & !to_delete;
}

bool Particle::InScreen() {
	if (collider == nullptr) return false;
	return (position.x + collider->rect.w < App->render->viewport.w - App->render->camera.x) && position.x > 0 &&
		position.y + collider->rect.h < App->render->viewport.h - App->render->camera.y && position.y > 0;
}
