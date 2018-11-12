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

	matrix[COLLIDER_RAY_RIGHT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_RAY_LEFT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_RAY_UP][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_RAY_DOWN][COLLIDER_FLOOR] = true;

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
	
		if (colliders[i] == nullptr /*|| colliders[i]->type != COLLIDER_NONE*/)
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
			if (c1->CheckCollision(c2->rect))
			{
				if (matrix[c1->type][c2->type] && c1->callback) {

					//LOG("COLLIDED first if");
					c1->callback->OnCollision(c1, c2);
				}

				if (matrix[c2->type][c1->type] && c2->callback) {

					//LOG("COLLIDED second if");
					c2->callback->OnCollision(c2, c1);
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
			case COLLIDER_RAY_RIGHT:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			case COLLIDER_RAY_LEFT:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			case COLLIDER_RAY_UP:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			case COLLIDER_RAY_DOWN:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				break;
			}

	}

	//Drawing polylines
	/*SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

	for (int i = 0; i < App->collision->n_lines; i++) {
		int offx = polylines[i][0] + App->render->camera.x;
		int offy = polylines[i][1] + App->render->camera.y;

		for (int j = 2; j < n_lines_col[i]; j += 2) {
			SDL_RenderDrawLine(App->render->renderer,
				polylines[i][j] + offx, polylines[i][j + 1] + offy,
				polylines[i][j + 2] + offx, polylines[i][j + 3] + offy);
		}
	}*/

	
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	CleanColliders();

	//CleanPolylines();

	return true;
}

void j1Collision::CleanColliders()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		if (colliders[i] != nullptr && colliders[i]->type != COLLIDER_PLAYER) {
			colliders[i] = nullptr;
		}
}

//void j1Collision::CleanPolylines()
//{
//	n_lines = 0;
//	for (int i = 0; i < n_lines; ++i) {
//		for (int j = 0; j < n_lines_col[i]; ++j)
//			polylines[i][j] = 0;
//	}
//}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);

			if (type == COLLIDER_PLAYER) {
				if (player_collider == nullptr) {
					player_collider = ret;
				}
				else {
					LOG("THERE CAN ONLY BE ONE PLAYER COLLIDER ERROR ADDING NEW COLLIDER");
				}
			}

			break;
		}
	}

	return ret;
}
//void j1Collision::AddPolyLine(int startX, int startY, const char* line)
//{
//	if (n_lines >= MAX_LINES) {
//		LOG("MAX LINES ACHIEVED");
//		return;
//	}
//	char* buffer = new char[6];
//	int point;
//	int j = 0;
//	int it = 0;
//
//	polylines[n_lines][it++] = startX;
//	polylines[n_lines][it++] = startY;
//
//	for (uint i = 0; i <= strlen(line); i++) {
//		char k = line[i];
//
//		if (k == ',') {				// Had finished parsing x point, time to add it
//			polylines[n_lines][it++] = atoi(buffer);
//			while (j > 0) {				// Reset buffer and j
//				buffer[--j] = 0;
//			}
//		}
//		else if (k == ' ' || k == '\0') {			// Had finished parsing y point, time to add it
//			polylines[n_lines][it++] = atoi(buffer);// +offy;
//			while (j > 0) {				// Reset buffer and j
//				buffer[--j] = 0;
//			}
//		}
//		else {							// Parsing number
//			buffer[j++] = line[i];
//		}
//	}
//	n_lines_col[n_lines] = it - 1;
//	n_lines += 1;
//	
//}


bool Collider::CheckCollision(const SDL_Rect& r) const
{
	if ((r.x > rect.x + rect.w) || (r.y > rect.y + rect.h)) { return false; }

	if ((rect.x > r.x + r.w) || (rect.y > r.y + r.h)) { return false; }
	return true;
}

bool Collider::CheckRectLineCollision(int x1, int y1, int x2, int y2) const
{
	// check if the line has hit any of the rectectangle's sides
	boolean left =   CheckLineLine(x1, y1, x2, y2, rect.x, rect.y, rect.x, rect.y + rect.h);
	boolean right =  CheckLineLine(x1, y1, x2, y2, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
	boolean top =    CheckLineLine(x1, y1, x2, y2, rect.x, rect.y, rect.x + rect.w, rect.y);
	boolean bottom = CheckLineLine(x1, y1, x2, y2, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);

	// if ANY of the above are true, the line
	// has hit the rectectangle
	if (left || right || top || bottom) {
		return true;
	}
	return false;
}

bool CheckLineLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	float uADiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uBDiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));

	float uA = (uADiv == 0) ? 0 : ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / uADiv;
	float uB = (uBDiv == 0) ? 0 : ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / uBDiv;
	

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) return true;
	return false;
}
