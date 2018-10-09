#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

//TO TEST
struct Properties {
	bool draw;
	bool collider;
};

struct MapLayer {
	p2SString	name;
	uint		width;
	uint		height;
	uint*		data;
	Properties	properties;

	// Translates 2 dimension to the id of the one dimension array
	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint GetMapId(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

struct TileSet
{
	// Receives a tile id and returns it's Rectfind, 
	// the Rect associated with a specific tile id
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;
	SDL_Color			background_color;
	MapTypes			type;
	p2List<TileSet*>	tilesets;
	p2List<MapLayer*>	map_layers;
};

class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called each loop iteration
	void Draw();


	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	// Translates x,y coordinates from map positions to world positions
	iPoint MapToWorld(int x, int y) const;

	iPoint WorldToMap(int x, int y) const;

	TileSet * GetTilesetFromTileId(int id) const;

	p2SString DebugToString() const;

private:

	// Load map general properties
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);

public:
	
	MapData				data;
	p2List<p2SString>	maps_path;

private:

	pugi::xml_document	map_doc;
	p2SString			folder;
	bool				map_loaded;

};

#endif // __j1MAP_H__