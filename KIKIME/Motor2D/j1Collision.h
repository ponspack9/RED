#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#define MAX_COLLIDERS	350
#define MAX_LINES		256
#define MAX_LINE_LENGTH 50

#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "p2Point.h"
#include "p2List.h"



enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_PLAYER,
	COLLIDER_DEATH,
	COLLIDER_START,
	COLLIDER_END,

	COLLIDER_SPAWN,
	COLLIDER_SPAWN_COIN,
	COLLIDER_COIN,

	COLLIDER_DEBUG,

	COLLIDER_MAX
};
struct Collider
{
	SDL_Rect rect;

	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Module* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}
	Collider(Collider* c) :
		rect(c->rect),
		type(c->type),
		callback(c->callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}
	void operator += (Collider* c) {
		rect.x = c->rect.x;
		rect.y = c->rect.y;
		rect.w = c->rect.w;
		rect.h = c->rect.h;
		type = COLLIDER_NONE;
	}

	bool CheckCollision(const SDL_Rect& r) const;
	//bool CheckRectLineCollision(int x1, int y1, int x2, int y2) const;
};

	//bool CheckLineLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);


class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate();
	bool Start();
	bool Update(float dt);
	bool CleanUp();

	void CleanColliders();
	//void CleanPolylines();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	//void AddPolyLine(int startX, int startY, const char * c);
	void Draw();
	//void DrawPolylines();

	//int polylines[MAX_LINES][MAX_LINE_LENGTH];
	//int n_lines = 0;
	//int n_lines_col[MAX_LINES];
public:

	Collider * colliders[MAX_COLLIDERS];
	int active;

private:

	bool matrix[COLLIDER_MAX][COLLIDER_MAX] = { false };
};

#endif // __j1COLLISION_H__