#ifndef __j1DEBUG_H__
#define __j1DEBUG_H__

#include "j1Module.h"


class j1Debug :	public j1Module
{
public:
	j1Debug();

	~j1Debug();

	bool PostUpdate();

	bool Update(float dt);

};

#endif