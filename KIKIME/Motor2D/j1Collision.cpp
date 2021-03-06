#include "j1App.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "j1Debug.h"
#include "p2Log.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_START]  = false;
	matrix[COLLIDER_PLAYER][COLLIDER_SPAWN]	 = false;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH]  = true;
	matrix[COLLIDER_PLAYER][COLLIDER_END]    = true;
	matrix[COLLIDER_PLAYER][COLLIDER_COIN]	 = true;
	//matrix[COLLIDER_DEBUG][COLLIDER_D]

	CleanPolylines();

	name.create("collisions");
	active = 0;
}

// Destructor
j1Collision::~j1Collision(){}

bool j1Collision::PreUpdate()
{
	BROFILER_CATEGORY("Collision->PreUpdate", Profiler::Color::Green)
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < active; ++i)
	{
		//if (colliders[i] != nullptr)
		//LOG("TYPE: %d   TO_DELETE %d", colliders[i]->type, colliders[i]->to_delete);
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}
	
	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < active; ++i)
	{
	
		if (colliders[i] == nullptr /*|| colliders[i]->type != COLLIDER_NONE*/)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		//RECT COLLISIONS
		for (uint k = i + 1; k < active; ++k)
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
		if (c1->type == COLLIDER_PLAYER) {

			for (int i = 0; i < n_lines; i++) {
				int offx = App->render->camera.x;
				int offy = App->render->camera.y;

				for (int j = 0; j <= n_lines_col[i]; j += 2) {
					if (c1->CheckRectLineCollision(polylines[i][j] + offx, polylines[i][j + 1] + offy,
						polylines[i][j + 2] + offx, polylines[i][j + 3] + offy)) {
						c1->callback->OnCollisionLine();
						break;
					}
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
	BROFILER_CATEGORY("Collision->Update", Profiler::Color::Green)

	if (App->debug->show_colliders) Draw();

	return true;
}

void j1Collision::Draw()
{
	Uint8 alpha = 80;

	/*int death_colliders = 0;
	int coin_colliders = 0;
	int enemies_colliders = 0;
	int start_colliders = 0;
	int end_colliders = 0;
	int player_colliders = 0;
	int none_colliders = 0;*/

	for (uint i = 0; i < active; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

			switch (colliders[i]->type)
			{
			case COLLIDER_NONE:
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
				//none_colliders++;
				break;
			case COLLIDER_PLAYER:
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
				//player_colliders++;
				break;
			case COLLIDER_DEATH:
				App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
				//death_colliders++;
				break;
			case COLLIDER_START:
				App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
				//start_colliders++;
				break;
			case COLLIDER_END:
				App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
				//end_colliders++;
				break;
			case COLLIDER_COIN:
				App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
				//coin_colliders++;
				break;
			}
	}
	/*LOG("COLLIDERS COLLISION");
	LOG("DEATH: %d", death_colliders);
	LOG("COIN: %d", coin_colliders);
	LOG("START: %d", start_colliders);
	LOG("END: %d", end_colliders);
	LOG("PLAYER: %d", player_colliders);
	LOG("NONE: %d", none_colliders);*/
	DrawPolylines();
}

// Called before quitting
bool j1Collision::CleanUp()
{
	BROFILER_CATEGORY("Collision->CleanUp", Profiler::Color::Green)
	LOG("Freeing all colliders");

	CleanColliders();

	CleanPolylines();

	return true;
}

void j1Collision::CleanColliders()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		if (colliders[i] != nullptr) {
			delete colliders[i];
			colliders[i] = nullptr;
		}
	active = 0;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			if (i >= active) active += 1;
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

/// POLYLINES -------------------------------------------------

void j1Collision::DrawPolylines() {
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);

	for (int i = 0; i < n_lines; i++) {
		int offx = App->render->camera.x;
		int offy = App->render->camera.y;

		for (int j = 0; j <= n_lines_col[i]; j += 2) {
			SDL_RenderDrawLine(App->render->renderer,
				polylines[i][j] + offx, polylines[i][j + 1] + offy,
				polylines[i][j + 2] + offx, polylines[i][j + 3] + offy);
		}
	}
}

void j1Collision::AddPolyLine(int startX, int startY, const char* line)
{
	if (n_lines >= MAX_LINES) {
		LOG("MAX LINES ACHIEVED");
		return;
	}
	char* buffer = new char[6];
	int point;
	int j = 0;
	int it = 0;

	polylines[n_lines][it++] = startX;
	polylines[n_lines][it++] = startY;

	for (uint i = 4; i <= strlen(line); i++) {
		char k = line[i];

		if (k == ',') {				// Had finished parsing x point, time to add it
			polylines[n_lines][it++] = atoi(buffer) + startX;
			while (j > 0) {				// Reset buffer and j
				buffer[--j] = 0;
			}
		}
		else if (k == ' ' || k == '\0') {			// Had finished parsing y point, time to add it
			polylines[n_lines][it++] = atoi(buffer) + startY;// +offy;
			while (j > 0) {				// Reset buffer and j
				buffer[--j] = 0;
			}
		}
		else {							// Parsing number
			buffer[j++] = line[i];
		}
	}

	n_lines_col[n_lines] = (it%2==0)?it/2:it/2 +1;
	n_lines += 1;
	RELEASE_ARRAY(buffer);
	
}

void j1Collision::CleanPolylines()
{
	n_lines = 0;
	for (int i = 0; i < MAX_LINES; ++i) {
		n_lines_col[i] = 0;
		for (int j = 0; j < MAX_LINE_LENGTH; ++j)
			polylines[i][j] = 0;
	}
}



bool Collider::CheckRectLineCollision(int x1, int y1, int x2, int y2) const
{
	int offx = App->render->camera.x;
	int offy = App->render->camera.y;

	// check if the line has hit any of the rectangle's sides
	bool left =   CheckLineLine(x1, y1, x2, y2, rect.x+offx, rect.y+offy, rect.x + offx, rect.y + rect.h + offy);
	bool right =  CheckLineLine(x1, y1, x2, y2, rect.x + rect.w + offx, rect.y + offy, rect.x + rect.w + offx, rect.y + rect.h + offy);
	bool top =    CheckLineLine(x1, y1, x2, y2, rect.x + offx, rect.y + offy, rect.x + rect.w + offx, rect.y + offy);
	bool bottom = CheckLineLine(x1, y1, x2, y2, rect.x + offx, rect.y + rect.h + offy, rect.x + rect.w + offx, rect.y + rect.h + offy);

	return left || right || top || bottom;
}

bool CheckLineLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	float uADiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uBDiv = ((y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1));
	float uA = (uADiv == 0) ? 0 : ((x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)) / uADiv;
	float uB = (uBDiv == 0) ? 0 : ((x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)) / uBDiv;

	// if uA and uB are between 0-1, lines are colliding
	
	return uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1;
}

