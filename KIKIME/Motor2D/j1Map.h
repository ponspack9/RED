#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"
#include "SDL/include/SDL_pixels.h"

const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};

struct Properties
{
	struct Property
	{
		p2SString name;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while (item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

struct ImageLayer {
	p2SString		name;
	SDL_Texture*	texture;
	int				tex_width;
	int				tex_height;
	int				offset_x;
	int				offset_y;
	float			parallax_speed;

};

struct MapLayer {
	p2SString	name;
	uint		width;
	uint		height;
	uint*		data;
	Properties	properties;

	// Translates 2 dimension to the id of the one dimension array
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
	p2List<ImageLayer*> image_layers;
};

class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	bool CreateWalkabilityMap(pugi::xml_node & node, int & width, int & height, uchar ** buffer) const;

	bool LoadProperties(pugi::xml_node & node, Properties & properties);

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

	void CleanMap();

private:

	// Load map general properties
	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadImageLayer(pugi::xml_node& node, ImageLayer* layer);

public:
	
	MapData						data;
	p2List<p2SString>			maps_path;
	p2List<p2SString>			walk_maps_path;
	p2List_item<p2SString>*		current_map;
	p2List_item<p2SString>*		current_walk_map;
	iPoint						world_limits;
	Collider*					start_collider;
	Collider*					end_collider;

	bool						map_loaded;

private:

	pugi::xml_document	map_doc;
	p2SString			folder;

};

#endif // __j1MAP_H__