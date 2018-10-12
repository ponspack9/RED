#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Collision.h"
#include "j1Debug.h"
#include "p2Log.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_GROUND] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;

	name.create("collisions");
}

// Destructor
j1Collision::~j1Collision(){}

bool j1Collision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect))
			{
				if (matrix[c1->type][c2->type] && c1->callback) {

					//LOG("COLLIDED first if");
					//c1->callback->OnCollision(c1, c2);
				}

				if (matrix[c2->type][c1->type] && c2->callback) {

					//LOG("COLLIDED second if");
					//c2->callback->OnCollision(c2, c1);
				}	
			}
		}
	}

	return true;
}
bool j1Collision::Start() {
	return true;
}

// Called before render is available
bool j1Collision::Update(float dt)
{

	if (App->debug->show_colliders) Draw();

	return true;
}

void j1Collision::Draw()
{
	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		//LOG("x: %d y: %d w: %d h: %d", colliders[i]->rect.x, colliders[i]->rect.y, colliders[i]->rect.w, colliders[i]->rect.h);

			switch (colliders[i]->type)
			{
			case COLLIDER_NONE:
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
				break;
			case COLLIDER_PLAYER:
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
				break;
			case COLLIDER_GROUND:
				App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
				break;
			case COLLIDER_DEATH:
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
				break;
			}
	}

	p2List_item<PolyLine*>* line = polylines.start;
	SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	int a = App->collision->n_lines;
	for (line; line != polylines.end->prev; line = line->next) {
		p2List_item<iPoint>* p = line->data->points.start;
		int offsetx = line->data->start.x + App->render->camera.x;
		int offsety = line->data->start.y + App->render->camera.y;
		SDL_RenderDrawLine(App->render->renderer, p->data.x + offsetx, p->data.y + offsety,
					p->next->data.x + offsetx, p->next->data.y + offsety);

		for (int i=0;i<line->data->points.count()-1;i++){

			SDL_RenderDrawLine(App->render->renderer, p->data.x + offsetx, p->data.y + offsety, 
				p->next->data.x + offsetx, p->next->data.y + offsety);
			//int a = 1;
			p = p->next;
		}
		SDL_RenderDrawLine(App->render->renderer, p->data.x + offsetx, p->data.y + offsety,
			line->data->points.start->data.x + offsetx, line->data->points.start->data.y + offsety);

	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		RELEASE(colliders[i]);
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}


bool Collider::CheckCollision(const SDL_Rect& r) const
{

	if ((r.x > rect.x + rect.w) || (r.y > rect.y + rect.h)) { return false; }

	if ((rect.x > r.x + r.w) || (rect.y > r.y + r.h)) { return false; }

	return true;
}