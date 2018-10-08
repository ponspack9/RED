#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Map.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

j1Map::~j1Map()
{}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;

	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));

	return rect;
}

inline uint MapLayer::GetMapId(int x, int y) const
{
	return x + (y * width);
}

bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");

	folder.create(config.child("folder").child_value());

	return true;
}

void j1Map::Draw()
{
	if(!map_loaded)
		return;

	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	p2List_item<MapLayer*>* layer; 
	layer = data.map_layers.start;
	for (layer; layer; layer = layer->next) {
		for (int y = 0; y < data.height; ++y) {
			for (int x = 0; x < data.width; ++x) {

				uint id = layer->data->GetMapId(x, y);

				id = layer->data->data[id];

				if (id != 0) {
					SDL_Rect *rect = &item->data->GetTileRect(id);
					iPoint pos = MapToWorld(x, y);
					App->render->Blit(item->data->texture, pos.x, pos.y, rect);
				}
			}
		}
	}

}

bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* layer;
	layer = data.map_layers.start;

	while (layer != NULL)
	{
		RELEASE(layer->data);
		layer = layer->next;
	}
	data.map_layers.clear();

	map_doc.reset();

	return true;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint pos;

	pos.x = x * data.tile_width;
	pos.y = y * data.tile_height;

	return pos;
}

void j1Map::MapToWorldRef(int &x, int &y) const
{
	x = x * data.tile_width;
	y = y * data.tile_height;
}

iPoint j1Map::WorldToMap(int x, int y) const 
{
	iPoint pos;

	pos.x = x / data.tile_width;
	pos.y = y / data.tile_height;

	return pos;
}

void j1Map::WorldToMapRef(int &x, int &y) const
{
	x = x / data.tile_width;
	y = y / data.tile_height;
}

p2SString j1Map::DebugToString() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_pos = WorldToMap(x, y);
	
	int map_id  = data.map_layers.start->data->GetMapId(map_pos.x, map_pos.y);
	int tile_id = data.map_layers.start->data->data[map_id + abs(App->render->camera.x/data.tile_width)];

	// Loading info to title FLASHES WINDOW ICON IN TASK BAR
	p2SString ret_string("Map: %dx%d Tiles: %dx%d Tilesets: %d Mouse [%d,%d] Rect [%d,%d] MapID: %d TilesetID: %d Camera.x: %d offsetX: %d",
		data.width, data.height,
		data.tile_width, data.tile_height,
		data.tilesets.count(),
		x, y,
		map_pos.x,map_pos.y, 
		map_id,tile_id,
		App->render->camera.x,
		abs(App->render->camera.x / data.tile_width));

	return ret_string;
}

bool j1Map::Load(const char* file_name)
{
	pugi::xml_parse_result result = App->LoadXML(map_doc, PATH(folder.GetString(),file_name));

	bool ret = (result != NULL);

	// Load general info (ERROR handled by LoadXML)
	if(ret)
	{
		ret = LoadMap();
	
		// Load all tilesets info
		pugi::xml_node tileset = map_doc.child("map").child("tileset");
		for(tileset; tileset && ret; tileset = tileset.next_sibling("tileset"))
		{
			TileSet* set = new TileSet();

			if(ret)	ret = LoadTilesetDetails(tileset, set);
			if(ret)	ret = LoadTilesetImage(tileset, set);
			if(ret) data.tilesets.add(set);

		}

		// Load layer info 
		pugi::xml_node layer = map_doc.child("map").child("layer");
		for (layer; layer && ret; layer = layer.next_sibling("layer"))
		{
			MapLayer* set = new MapLayer();

			if (ret) ret = LoadLayer(layer, set);
			if (ret) data.map_layers.add(set);
		}
	}
	 map_loaded = ret;

	// Logs => data has been loaded properly
	if(map_loaded)
	{
		//Logging map info
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		// Logging layers info
		p2List_item<MapLayer*>* item_layer = data.map_layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	return map_loaded;
}

bool j1Map::LoadMap()
{
	pugi::xml_node map = map_doc.child("map");
	bool ret = map != NULL;

	if(ret)
	{
		data.width			= map.attribute("width").as_int();
		data.height			= map.attribute("height").as_int();
		data.tile_width		= map.attribute("tilewidth").as_int();
		data.tile_height	= map.attribute("tileheight").as_int();
		p2SString bg_color	( map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if		(orientation == "orthogonal")	data.type = MAPTYPE_ORTHOGONAL;
		else if (orientation == "isometric")	data.type = MAPTYPE_ISOMETRIC;
		else if (orientation == "staggered")	data.type = MAPTYPE_STAGGERED;
		else									data.type = MAPTYPE_UNKNOWN;
	}
	else
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = tileset_node != NULL;
	if (ret){
		set->name.create	  ( tileset_node.attribute("name").as_string());
		set->firstgid		  = tileset_node.attribute("firstgid").as_int();
		set->tile_width		  = tileset_node.attribute("tilewidth").as_int();
		set->tile_height	  = tileset_node.attribute("tileheight").as_int();
		set->margin			  = tileset_node.attribute("margin").as_int();
		set->spacing		  = tileset_node.attribute("spacing").as_int();
		pugi::xml_node offset = tileset_node.child("tileoffset");

		if(offset != NULL)
		{
			set->offset_x = offset.attribute("x").as_int();
			set->offset_y = offset.attribute("y").as_int();
		}
		else
		{
			set->offset_x = 0;
			set->offset_y = 0;
		}
	}
	else
	{
		LOG("ERROR loading tileset details: node is NULL");
	} 

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = tileset_node != NULL;

	if (ret) {
		pugi::xml_node image = tileset_node.child("image");
		ret = (image != NULL);

		if (ret) {

			set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
			int w, h;
			SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
			set->tex_width = image.attribute("width").as_int();

			if (set->tex_width <= 0)
			{
				set->tex_width = w;
			}

			set->tex_height = image.attribute("height").as_int();

			if (set->tex_height <= 0)
			{
				set->tex_height = h;
			}

			set->num_tiles_width = set->tex_width / set->tile_width;
			set->num_tiles_height = set->tex_height / set->tile_height;
		}
		else
		{
			LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		}
	}
	else
	{
		LOG("ERROR loading tileset image: node is NULL");
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node & node, MapLayer * layer)
{
	bool ret = node != NULL;

	if (ret) {
		layer->name = node.attribute("name").as_string();
		layer->height = node.attribute("height").as_int();
		layer->width = node.attribute("width").as_int();
		layer->data = new uint[layer->width*layer->height];

		memset(layer->data, 0, (sizeof(uint))*layer->width*layer->height);

		int i = 0;
		pugi::xml_node inode = map_doc.child("map").child("layer").child("data").child("tile");

		for (inode; inode; inode = inode.next_sibling("tile"))
		{
			layer->data[i++] = inode.attribute("gid").as_uint();
		}
	}
	else
	{
		LOG("ERROR loading layer: node is NULL");
	}

	return ret;
}
