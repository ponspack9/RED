#pragma once
#include "j1Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "j1Collision.h"

#define MAX_ACTIVE_PARTICLES 50

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

struct Particle
{
	Collider* collider = nullptr;
	Animation anim;
	uint fx = 0;
	iPoint position;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;

	bool fx_played = false;
	bool to_delete = false;

	Particle();
	Particle(const Particle& p);

	bool Update();

	bool InScreen();
};

class j1Particles : public j1Module
{
public:
	j1Particles();
	~j1Particles() {}

	bool Start();
	bool Awake(pugi::xml_node& conf);
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(Collider* c1, Collider* c2);

	Particle* AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0);
	Particle* AddParticle(const Particle& particle, iPoint pos,   COLLIDER_TYPE collider_type = COLLIDER_NONE, Uint32 delay = 0);

	void Reset();

private:

	p2SString graphics_path;

public:

	SDL_Texture* graphics = nullptr;

	Particle* active[MAX_ACTIVE_PARTICLES];	
	
	Particle green_diamond;
	Particle blue_diamond;
	Particle heart;
};