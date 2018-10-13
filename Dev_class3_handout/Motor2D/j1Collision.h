#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#define MAX_COLLIDERS 150
#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "p2Point.h"
#include "p2List.h"



enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_PLAYER,
	COLLIDER_FLOOR,
	COLLIDER_WALL,
	COLLIDER_DEATH,

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

	~Collider() {
		//RELEASE(callback);
	}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	bool CheckCollision(const SDL_Rect& r) const;
	bool CheckRectLineCollision(int x1, int y1, int x2, int y2) const;
};

	bool CheckLineLine(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

struct PolyLine {
	p2List<iPoint>  points;
	iPoint			 start;
};


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
	void CleanPolylines();

	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback = nullptr);
	void Draw();

	p2List<PolyLine*>	polylines;
	int n_lines = 0;
	
private:

	Collider * colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
};

#endif // __j1COLLISION_H__