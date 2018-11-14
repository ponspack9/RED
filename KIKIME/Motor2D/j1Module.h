// ----------------------------------------------------
// j1Module.h
// Interface for all engine modules
// ----------------------------------------------------

#ifndef __j1MODULE_H__
#define __j1MODULE_H__

#include "p2SString.h"
#include "PugiXml\src\pugixml.hpp"

class j1App;
struct Collider;

class j1Module
{
public:

	j1Module() : active(false)
	{}

	void Init()
	{
		active = true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			//Awake();
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			CleanUp();
			active = false;
		}
	}

	virtual void OnCollision(Collider*, Collider*) {}

	virtual void OnCollisionLine( int, int, int, int){}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	// Called on finish update from App
	virtual bool Save(pugi::xml_node& node) {
		return true;
	}

	// Called on finish update from App
	virtual bool Load(pugi::xml_node& node) {
		return true;
	}

public:

	p2SString	name;
	bool		active;

};

#endif // __j1MODULE_H__