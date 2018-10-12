#ifndef __j1DEBUG_H__
#define __j1DEBUG_H__

#include "j1Module.h"


class j1Debug :	public j1Module
{
public:
	j1Debug();

	~j1Debug();

	bool Awake(pugi::xml_node& config);

	bool PostUpdate();

	bool Update(float dt);


public:

	iPoint camera_motion;
	iPoint last_motion;
	iPoint zero;
	bool start_motion;
	bool show_colliders;

};

#endif