#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

typedef unsigned int Uint;

// TODO 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
// ----------------------------------------------------
struct tileset
{
	Uint firstgid = 1,
		 tile_w = 32,
		 tile_h = 32,
		 spacing = 1,
		 margin = 1;
	p2SString* name = nullptr;
};

// TODO 1: Create a struct needed to hold the information to Map node
struct Map
{
	enum renderOrder
	{
		RIGHT_DOWN,
		RIGHT_UP,
		LEFT_DOWN,
		LEFT_UP,
		_ERROR
	}order;
	
	enum orientation
	{
		ISOMETRIC,
		ORTHOGONAL,
		_ERROR
	}orientation;

	float version = 1.0;
	Uint w = 50,
		 h = 15,
		 tile_w = 32,
		 tile_h = 32,
		 nextobjectid = 1;

};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

private:

	bool LoadMap();

public:
	// TODO 1: Add your struct for map info as public for now
	struct Map* map;
	
private:

	p2List<j1Module*>	attributes;
	pugi::xml_document	map_file;
	p2SString			folder;
	bool				map_loaded;
};

#endif // __j1MAP_H__