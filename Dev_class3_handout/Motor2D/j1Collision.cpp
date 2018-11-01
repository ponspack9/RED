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
	
	//Works only checking the player, as we only have one player that interacts with everything,
	// we should only check it,, forgetting about the for
	// avoid checking unnecessary colliders
	if (player_collider == nullptr) {
		LOG("NO player collider, avoiding checking collisions");
		return true;
	}

	//RECT COLLISIONS
	for (uint k = 0; k < MAX_COLLIDERS; ++k)
	{
		// stop when finds empty colliders
		if (colliders[k] == nullptr)
			break;

		Collider* c2 = colliders[k];

		if (matrix[player_collider->type][c2->type] && player_collider->CheckCollision(c2->rect))
		{
			if (player_collider->callback) {					
				//LOG("COLLIDED first if");
				player_collider->callback->OnCollision(player_collider, c2);
			}
			if (c2->callback) {
				//LOG("COLLIDED second if");
				c2->callback->OnCollision(c2, player_collider);
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

	//Drawing polylines
	SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);

	for (int i = 0; i < App->collision->n_lines; i++) {
		int offx = polylines[i][0] + App->render->camera.x;
		int offy = polylines[i][1] + App->render->camera.y;

		for (int j = 2; j < n_lines_col[i]; j += 2) {
			SDL_RenderDrawLine(App->render->renderer,
				polylines[i][j] + offx, polylines[i][j + 1] + offy,
				polylines[i][j + 2] + offx, polylines[i][j + 3] + offy);
		}
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
		if (colliders[i] != nullptr && colliders[i]->type != COLLIDER_PLAYER) {
			colliders[i] = nullptr;
		}
}

void j1Collision::CleanPolylines()
{
	n_lines = 0;
}

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
void j1Collision::AddPolyLine(int startX, int startY, const char* line)
{
	char* buffer = new char[6];
	int point;
	int j = 0;
	int it = 0;

	polylines[n_lines][it++] = startX;
	polylines[n_lines][it++] = startY;

	for (uint i = 0; i <= strlen(line); i++) {
		char k = line[i];

		if (k == ',') {				// Had finished parsing x point, time to add it
			polylines[n_lines][it++] = atoi(buffer);
			while (j > 0) {				// Reset buffer and j
				buffer[--j] = 0;
			}
		}
		else if (k == ' ' || k == '\0') {			// Had finished parsing y point, time to add it
			polylines[n_lines][it++] = atoi(buffer);// +offy;
			while (j > 0) {				// Reset buffer and j
				buffer[--j] = 0;
			}
		}
		else {							// Parsing number
			buffer[j++] = line[i];
		}
	}
	n_lines_col[n_lines] = it - 1;

	if (n_lines + 1 > MAX_LINES) {
		LOG("MAX LINES ACHIEVED");
	}
	else {
		n_lines += 1;
	}
}


bool Collider::CheckCollision(const SDL_Rect& r) const
{
	//return CheckLineLine(rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h, r.x, r.y+r.h, r.w+r.x, r.h+r.y);
	if ((r.x > rect.x + rect.w) || (r.y > rect.y + rect.h)) { return false; }

	if ((rect.x > r.x + r.w) || (rect.y > r.y + r.h)) { return false; }
	return true;

	/*if (rect.x || (rect.x + rect.w) > (r.x ) && (rect.x || (rect.x + rect.w) < (r.x + r.w))) {
		if ((rect.y + rect.h) < (r.y)) {
			return false;
		}
	}
	if ((rect.y > (r.y)) || ((rect.y + rect.h) > (r.y))) {
		if (rect.x < r.x) {
			return true;
		}
	}

	if ((rect.y > (r.y)) || ((rect.y + rect.h) > (r.y))) {
		if (rect.x > r.x + (r.w - 5)) {
			return true;
		}
	}

	if (rect.x || (rect.x + rect.w) > (r.x ) && (rect.x || (rect.x + rect.w) < (r.x + r.w))) {
		if (rect.y > (r.y)) {
			return true;
		}
	}
	return false;*/

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
