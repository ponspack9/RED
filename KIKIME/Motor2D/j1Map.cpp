#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "Brofiler/Brofiler.h"
#include "j1Pathfinding.h"
#include <cmath>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");

	world_limits	= { 0,0 };
	start_collider	= nullptr;
	end_collider	= nullptr;
}

j1Map::~j1Map()
{}

bool j1Map::CreateWalkabilityMap(pugi::xml_node& node, int & width, int & height, uchar ** buffer) const
{

	width = node.attribute("width").as_int();
	height = node.attribute("height").as_int();

	uchar* map = new uchar[width*height];
	memset(map, 1, width*height);

	int i = 0;
	for (pugi::xml_node tile = node.child("data").child("tile"); tile; tile = tile.next_sibling("tile"))
	{
		int tile_value = tile.attribute("gid").as_int();
		map[i] = (tile_value > 0) ? 0 : 1;
		i++;
	}

	*buffer = map;

	return true;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	BROFILER_CATEGORY("Map->LoadProperties", Profiler::Color::LightCyan)
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.add(p);
		}
	}

	return ret;
}

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

bool j1Map::Awake(pugi::xml_node& config)
{
	//BROFILER_CATEGORY("Map->Awake", Profiler::Color::LightCyan)
	LOG("Loading Map Parser");

	folder.create(config.child("folder").child_value());

	LOG("MAPS: ");

	pugi::xml_node level_node = config.child("level");
	int i = 0;
	for (level_node; level_node; level_node = level_node.next_sibling("level")) {
		maps_path.add(level_node.child_value());
		
		LOG("%s", maps_path[i++].GetString());
	}
	current_map = maps_path.start;

	return true;
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Map->Draw", Profiler::Color::LightCyan)
	if(!map_loaded)
		return;

	//Draw background
	p2List_item<ImageLayer*>* imagelayer = data.image_layers.start;
	for (imagelayer; imagelayer; imagelayer = imagelayer->next)
	{
		SDL_Rect r = { 0,0,imagelayer->data->tex_width,imagelayer->data->tex_height };
		App->render->Blit(imagelayer->data->texture, imagelayer->data->offset_x, imagelayer->data->offset_y, &r, imagelayer->data->parallax_speed);
	}
	//imagelayer->~p2List_item();

	p2List_item<MapLayer*>* layer = data.map_layers.start;

	for (layer; layer; layer = layer->next)
	{
		//LOG("LAYER NAMEEEEEEEEEEE: %s", layer->data->name.GetString());
		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{

				int tile_id = layer->data->GetMapId(x, y);

				bool flipped_horizontally	= (tile_id & FLIPPED_HORIZONTALLY_FLAG);
				bool flipped_vertically		= (tile_id & FLIPPED_VERTICALLY_FLAG);
				bool flipped_diagonally		= (tile_id & FLIPPED_DIAGONALLY_FLAG);


				tile_id &= ~(FLIPPED_HORIZONTALLY_FLAG |
					FLIPPED_VERTICALLY_FLAG |
					FLIPPED_DIAGONALLY_FLAG);

				if (tile_id > 0)
				{
					TileSet* tileset = GetTilesetFromTileId(tile_id);
					if (tileset != nullptr)
					{
						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);
						double angle = 0;
						if (flipped_horizontally)	angle += 180;
						if (flipped_vertically)		angle += 360;
						if (flipped_diagonally)		angle += 270;
						App->render->Blit(tileset->texture, pos.x, pos.y, &r,1,angle);
					}
				}
			}
		}
	}
	//layer->~p2List_item();
}

bool j1Map::Load(const char* file_name)
{
	BROFILER_CATEGORY("Map->Load", Profiler::Color::LightCyan)
	pugi::xml_parse_result result = App->LoadXML(map_doc, PATH(folder.GetString(),file_name));

	bool ret = (result != NULL);

	// Load general info (ERROR handled by LoadXML)
	if(ret)
	{
		ret = LoadMap();
		//if (ret) = LoadBackground();
	
		// Load all tilesets info
		pugi::xml_node tileset = map_doc.child("map").child("tileset");
		for(tileset; tileset && ret; tileset = tileset.next_sibling("tileset"))
		{
			LOG("TILESET NAME: %s", tileset.attribute("name").as_string());
			TileSet* set = new TileSet();

			if(ret)	ret = LoadTilesetDetails(tileset, set);
			if(ret)	ret = LoadTilesetImage(tileset, set);
			if(ret) data.tilesets.add(set);
		}
		
		// Load background info 
		pugi::xml_node imagelayer = map_doc.child("map").child("imagelayer");
		for (imagelayer; imagelayer && ret; imagelayer = imagelayer.next_sibling("imagelayer"))
		{
			ImageLayer* set = new ImageLayer();

			if (ret) ret = LoadImageLayer(imagelayer,set);
			if (ret) data.image_layers.add(set);
		}

		// Load layer info 
		pugi::xml_node layer = map_doc.child("map").child("layer");
		for (layer; layer && ret; layer = layer.next_sibling("layer"))
		{
			MapLayer* set = new MapLayer();
			p2SString name = layer.attribute("name").as_string();
			if (name == "Walkability")
			{
				int w, h;
				uchar* data = NULL;
				if (CreateWalkabilityMap(layer, w, h, &data))
					App->pathfinding->SetMap(w, h, data);
				else LOG("ERROR LOADING WALKABILITY MAP");
			}
			else {
				if (ret) ret = LoadLayer(layer, set);
				if (ret) data.map_layers.add(set);
			}
		}

		//BEGIN DEALING WITH MAP OBJECTS

		pugi::xml_node objectgroup = map_doc.child("map").child("objectgroup");
		int death_colliders = 0;
		int coin_colliders = 0;
		int enemies_colliders = 0;
		Collider* c = nullptr;
		for (objectgroup; objectgroup; objectgroup = objectgroup.next_sibling("objectgroup"))
		{
			p2SString group_name ( objectgroup.attribute("name").as_string());
			//LOG("OBJECTGROUP NAME: %s", objectgroup.attribute("name").as_string());

			pugi::xml_node object = objectgroup.child("object");
			pugi::xml_node polyobject = object.child("polygon");

			for (object; object; object = object.next_sibling("object")) {

				polyobject = object.child("polyline");

				if (polyobject == NULL) {
					polyobject = object.child("polygon");

					if (polyobject == NULL) {
						polyobject = object.child("ellipse");

						if (polyobject == NULL) {
							//ITS A QUAD
							SDL_Rect r = { object.attribute("x").as_int() , object.attribute("y").as_int() ,
										   object.attribute("width").as_int() ,object.attribute("height").as_int() };
							
							if (group_name == "Colliders_death")
							{
								c = App->collision->AddCollider(r, COLLIDER_DEATH);
								c->to_delete = false;
								death_colliders++;
							}
							else if (group_name == "Collider_start") {
								start_collider = App->collision->AddCollider(r, COLLIDER_START);
							}
							else if (group_name == "Collider_end") {
								end_collider = App->collision->AddCollider(r, COLLIDER_END);
							}
							else if (group_name == "Enemy_spawns") {
								c = App->collision->AddCollider(r, COLLIDER_SPAWN);
								c->to_delete = false;
								enemies_colliders++;
							}
							else if (group_name == "Coin_spawns") {
								c = App->collision->AddCollider(r, COLLIDER_SPAWN_COIN);
								c->to_delete = false;
								coin_colliders++;
							}
							continue;
						}
						//ellipse

					}
				}
				//It's a Polyline

				//Deal with the string to find each point
				//const char* c = polyobject.attribute("points").as_string();

				//App->collision->AddPolyLine(object.attribute("x").as_int(), object.attribute("y").as_int(),c);

				// END DEALING OBJECT NODE
			}
		}

	LOG("COLLIDERS");
	LOG("DEATH: %d", death_colliders);
	LOG("COIN: %d", coin_colliders);
	LOG("ENEMY: %d", enemies_colliders);
	}
	 map_loaded = ret;

	// Logs => data has been loaded properly
	if(map_loaded)
	{
		world_limits = { data.width * data.tile_width, data.height * data.tile_width };
		//Logging map info
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);
		LOG("world_limits: %d , %d", world_limits.x, world_limits.y);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("------------- Tileset -------------");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}
		item->~p2List_item();

		// Logging layers info
		p2List_item<MapLayer*>* item_layer = data.map_layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("------------- Layer -------------");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
		item_layer->~p2List_item();
	}

	return map_loaded;
}

int Properties::Get(const char* value, int default_value) const
{
	p2List_item<Property*>* item = list.start;

	while (item)
	{
		if (item->data->name == value)
			return item->data->value;
		item = item->next;
	}

	return default_value;
}

bool j1Map::LoadMap()
{
	BROFILER_CATEGORY("Map->LoadMap", Profiler::Color::LightCyan)
	pugi::xml_node map = map_doc.child("map");
	bool ret = map != NULL;

	if(ret)
	{
		data.width			= map.attribute("width").as_int();
		data.height			= map.attribute("height").as_int();
		data.tile_width		= map.attribute("tilewidth").as_int();
		data.tile_height	= map.attribute("tileheight").as_int();
		p2SString bg_color	( map.attribute("trans").as_string());

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
	BROFILER_CATEGORY("Map->LoadTilesetDetails", Profiler::Color::LightCyan)
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
	BROFILER_CATEGORY("Map->LoadTilesetImage", Profiler::Color::LightCyan)
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
	BROFILER_CATEGORY("Map->LoadLayer", Profiler::Color::LightCyan)
	bool ret = node != NULL;

	if (ret) {
		layer->name = node.attribute("name").as_string();
		layer->height = node.attribute("height").as_int();
		layer->width = node.attribute("width").as_int();
		layer->data = new uint[layer->width*layer->height];
		LoadProperties(node, layer->properties);

		memset(layer->data, 1, (sizeof(uint))*layer->width*layer->height);

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

bool j1Map::LoadImageLayer(pugi::xml_node & node, ImageLayer * set)
{
	BROFILER_CATEGORY("Map->LoadImageLayer", Profiler::Color::LightCyan)
	bool ret = node != NULL;

	if (ret) {
		pugi::xml_node image = node.child("image");
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
					
		}
		else
		{
			LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		}

		pugi::xml_node prop = node.child("properties");
		ret = (prop != NULL);
		if (ret) {
			set->parallax_speed = prop.first_child().attribute("value").as_float();
			//This needs to be iterated with a for trhough all properties
			//set->offset_x = offset.attribute("offsetx").as_int();
			//set->offset_y = offset.attribute("offsetx").as_int();
			set->offset_x = 0;
			set->offset_y = 0;
		}
		else
		{
			LOG("Error parsing tileset xml file: Cannot find 'properties' tag.");
		}
	}
	else
	{
		LOG("ERROR loading tileset image: node is NULL");
	}

	return ret;

}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	//BROFILER_CATEGORY("Map->GetTilesetFromTileId", Profiler::Color::LightCyan)
	p2List_item<TileSet*>* item = data.tilesets.end;

	for (item; item != nullptr; item = item->prev)
	{
		if (id >= item->data->firstgid)
		{
			return item->data;
		}
	}

	return nullptr;
}

bool j1Map::CleanUp()
{
	BROFILER_CATEGORY("Map->CleanUp", Profiler::Color::LightCyan)

	// Remove all tilesets /////////////////
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

	while(tileset != NULL)
	{
		App->tex->UnLoad(tileset->data->texture);
		//RELEASE(tileset->data);
		tileset = tileset->next;
	}
	data.tilesets.clear();
	tileset->~p2List_item();


	// Remove all maplayers /////////////////
	p2List_item<MapLayer*>* layer;
	layer = data.map_layers.start;

	while (layer != NULL)
	{
		RELEASE_ARRAY(layer->data->data);
		//RELEASE(layer->data);
		layer = layer->next;
	}
	data.map_layers.clear();
	layer->~p2List_item();


	//Remove image layers /////////////////
	p2List_item<ImageLayer*>* imagelayer;
	imagelayer = data.image_layers.start;

	while (imagelayer != NULL)
	{
		App->tex->UnLoad(imagelayer->data->texture);
		//RELEASE(imagelayer->data);
		imagelayer = imagelayer->next;
	}
	data.image_layers.clear();
	imagelayer->~p2List_item();


	map_doc.reset();
	map_loaded = false;

	return true;
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	//BROFILER_CATEGORY("Map->MapToWorld", Profiler::Color::LightCyan)
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (int)(x - y) * (data.tile_width /2);
		ret.y = (int)(x + y) * (data.tile_height /2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const 
{
	BROFILER_CATEGORY("Map->WorldToMap", Profiler::Color::LightCyan)
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2);
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

//p2SString j1Map::DebugToString() const
//{
//	BROFILER_CATEGORY("Map->DebugToString", Profiler::Color::LightCyan)
//	int x, y;
//	App->input->GetMousePosition(x, y);
//	iPoint map_pos = WorldToMap(x, y);
//	
//	//int map_id  = data.map_layers.start->data->GetMapId(map_pos.x, map_pos.y);
//	//int tile_id = data.map_layers.start->data->data[map_id + abs(App->render->camera.x/data.tile_width)];
//
//	// Loading info to title FLASHES WINDOW ICON IN TASK BAR
//	p2SString ret_string("Map: %dx%d Tiles: %dx%d Tilesets: %d Mouse [%d,%d] Rect [%d,%d] Camera.x: %d offsetX: %d",
//		data.width, data.height,
//		data.tile_width, data.tile_height,
//		data.tilesets.count(),
//		x, y,
//		map_pos.x,map_pos.y, 
//		//map_id,tile_id, MapID: %d TilesetID: %d
//		App->render->camera.x,
//		(data.tile_width > 0) ? abs(App->render->camera.x / data.tile_width): -5000);
//
//	return ret_string;
//}

void j1Map::CleanMap()
{
	BROFILER_CATEGORY("Map->CleanMap", Profiler::Color::LightCyan)
	LOG("CLEANING MAP");

	// Remove all tilesets /////////////////
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

	while (tileset != NULL)
	{
		App->tex->UnLoad(tileset->data->texture);
		//RELEASE(tileset->data);
		tileset = tileset->next;
	}
	data.tilesets.clear();
	tileset->~p2List_item();


	// Remove all maplayers /////////////////
	p2List_item<MapLayer*>* layer;
	layer = data.map_layers.start;

	while (layer != NULL)
	{
		RELEASE_ARRAY(layer->data->data);
		//RELEASE(layer->data);
		layer = layer->next;
	}
	data.map_layers.clear();
	layer->~p2List_item();


	//Remove image layers /////////////////
	p2List_item<ImageLayer*>* imagelayer;
	imagelayer = data.image_layers.start;

	while (imagelayer != NULL)
	{
		App->tex->UnLoad(imagelayer->data->texture);
		//RELEASE(imagelayer->data);
		imagelayer = imagelayer->next;
	}
	data.image_layers.clear();
	imagelayer->~p2List_item();

	//Removes all colliders
	App->collision->CleanColliders();
		//App->collision->CleanPolylines();
	

	map_doc.reset();
	map_loaded = false;
	LOG("CLEANING MAP DONE");
}
