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
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR]	 = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL]   = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH]  = true;
	matrix[COLLIDER_PLAYER][COLLIDER_START]  = false;
	matrix[COLLIDER_PLAYER][COLLIDER_END]    = true;

	/*matrix[COLLIDER_FLOOR][COLLIDER_PLAYER]  = true;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR]   = false;
	matrix[COLLIDER_FLOOR][COLLIDER_WALL]    = false;
	matrix[COLLIDER_FLOOR][COLLIDER_DEATH]   = false;

	matrix[COLLIDER_WALL][COLLIDER_PLAYER]   = true;
	matrix[COLLIDER_WALL][COLLIDER_FLOOR]    = false;
	matrix[COLLIDER_WALL][COLLIDER_WALL]     = false;
	matrix[COLLIDER_WALL][COLLIDER_DEATH]    = false;

	matrix[COLLIDER_DEATH][COLLIDER_PLAYER]  = true;
	matrix[COLLIDER_DEATH][COLLIDER_FLOOR]   = false;
	matrix[COLLIDER_DEATH][COLLIDER_WALL]    = false;
	matrix[COLLIDER_DEATH][COLLIDER_DEATH]   = false;*/

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
		// skip empty colliders or not player colliders, he is the only one interacting
		// would be more efficient to only have one or two player colliders and not to 
		// iterate through all colliders till find the plaayer's one???
		if (colliders[i] == nullptr /*|| colliders[i]->type != COLLIDER_PLAYER*/)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		//RECT COLLISIONS
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			/*if (matrix[c1->type][c2->type] && c1->CheckCollision(c2->rect) && c1->callback) {
				LOG("COLLIDED first if");
			}
			else if (matrix[c2->type][c1->type] && c2->CheckCollision(c1->rect) && c2->callback) {
				LOG("COLLIDED second if");
			}*/
			if (c1->CheckCollision(c2->rect))
			{
				if (matrix[c1->type][c2->type] && c1->callback) {
					
					LOG("COLLIDED first if");
					c1->callback->OnCollision(c1, c2);
				}

				if (matrix[c2->type][c1->type] && c2->callback) {

					LOG("COLLIDED second if");
					c2->callback->OnCollision(c2, c1);
				}	
			}

		}
		
		if (c1->type == COLLIDER_PLAYER) {
			
		p2List_item<PolyLine*>* line = polylines.start;
		for (line; line != polylines.end->prev; line = line->next) {
			p2List_item<iPoint>* p = line->data->points.start;
			int offsetx = line->data->start.x + App->render->camera.x;
			int offsety = line->data->start.y + App->render->camera.y;
			
			for (int i = 0; i < line->data->points.count() - 1; i++) {

				if (c1->CheckRectLineCollision(p->data.x + offsetx, p->data.y + offsety,
					p->next->data.x + offsetx, p->next->data.y + offsety)) {
					//LOG("COLLISION WITH LINE");
					c1->callback->OnCollisionLine(c1, p->data.x + offsetx, p->data.y + offsety,
						p->next->data.x + offsetx, p->next->data.y + offsety);
				}
				p = p->next;
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
			case COLLIDER_FLOOR:
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
				break;
			case COLLIDER_WALL:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
				break;
			case COLLIDER_DEATH:
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
				break;
			case COLLIDER_START:
				App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
				break;
			case COLLIDER_END:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			}

	}

	p2List_item<PolyLine*>* line = polylines.start;
	SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
	for (line; line != polylines.end->prev; line = line->next) {
		p2List_item<iPoint>* p = line->data->points.start;
		int offsetx = line->data->start.x + App->render->camera.x;
		int offsety = line->data->start.y + App->render->camera.y;

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

	CleanColliders();

	CleanPolylines();

	return true;
}

void j1Collision::CleanColliders()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		if (colliders[i] != nullptr) {
			colliders[i] = nullptr;
		}
}

void j1Collision::CleanPolylines()
{
	//Remove all lines
	p2List_item<PolyLine*>* line;
	line = polylines.start;

	while (line != NULL) {
		line->data->points.clear();
		RELEASE(line->data);
		line = line->next;
	}
	polylines.clear();
	line->~p2List_item();
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
	//return CheckLineLine(rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h, r.x, r.y+r.h, r.w+r.x, r.h+r.y);
	if ((r.x > rect.x + rect.w) || (r.y > rect.y + rect.h)) { return false; }

	if ((rect.x > r.x + r.w) || (rect.y > r.y + r.h)) { return false; }
	//float w = (rect.x + rect.w + r.w+r.x)/2;
	//float h = (rect.h + rect.y + r.y+r.h)/2;
	//float dx = ((rect.w + rect.x) / 2) - ((r.w+r.x) / 2); //center of rect
	//float dy = ((rect.h + rect.y) / 2) - ((r.h+r.y) / 2); //center of rect
	//
	//if (abs(dx) <= w && abs(dy) <= h) {
	//	//Collision
	//	/*float wy = ;
	//	float hx = h * dx;*/
	//	float wy = w * dy;
	//	float hx = h * dx;

	//	if (wy < hx)
	//		if (wy > -hx) {
	//			return true;
	//			/* collision at the top */
	//		}
	//		else {
	//			/* on the left */
	//		}
	//	else if (wy < -hx) {
	//			/* on the right */
	//	}
	//	else {
	//			/* at the bottom */
	//		
	//	}
	//}
	//return false;
	///*return rect.y + rect.h > r.y && rect.y + rect.h < r.y + r.h
	//	&& rect.x+rect.w >r.x && rect.x+rect.w < r.x+r.w;
	//return true;*/
}

bool Collider::CheckRectLineCollision(int x1, int y1, int x2, int y2) const
{
	// check if the line has hit any of the rectectangle's sides
	boolean left =   CheckLineLine(x1, y1, x2, y2, rect.x, rect.y, rect.x, rect.y + rect.h);
	boolean right =  CheckLineLine(x1, y1, x2, y2, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
	boolean top =    CheckLineLine(x1, y1, x2, y2, rect.x, rect.y, rect.x + rect.w, rect.y);
	boolean bottom = CheckLineLine(x1, y1, x2, y2, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);

	// if ANY of the above arecte trectue, the line
	// has hit the rectectangle
	if (left || right || top || bottom) {
		return true;
	}
	return false;
}

bool CheckLineLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	float uADiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	if (uADiv == 0) return false;
	float uBDiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	if (uBDiv == 0) return false;
	float uA = ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / uADiv;
	float uB = ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / uBDiv;

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) return true;
	return false;
}
