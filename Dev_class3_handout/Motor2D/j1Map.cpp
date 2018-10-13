#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Map.h"
#include "j1Collision.h"
#include <cmath>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

j1Map::~j1Map()
{}

//ImageLayer::~ImageLayer()
//{
//	App->tex->UnLoad(texture);
//}

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

			if (ret) ret = LoadLayer(layer, set);
			if (ret) data.map_layers.add(set);
		}

		//BEGIN TRIP POLYLINE -> TO IMPROVE (cant make it work with a matrix of int[][] (see optimization branch)

		pugi::xml_node objectgroup = map_doc.child("map").child("objectgroup");
		//LOG("OBJECTGROUP NAME: %s", objectgroup.attribute("name").as_string());
		int a = 0;
		for (objectgroup; objectgroup; objectgroup = objectgroup.next_sibling("objectgroup"))
		{
			p2SString group_name ( objectgroup.attribute("name").as_string());
			//LOG("OBJECTGROUP NAME: %s", objectgroup.attribute("name").as_string());
			pugi::xml_node object = objectgroup.child("object");
			pugi::xml_node polyobject = object.child("polygon");
			a++;
			
			for (object; object; object = object.next_sibling("object")) {
				bool rotation = false;
				double alpha = 0;

				polyobject = object.child("polyline");

				if (polyobject == NULL) {
					polyobject = object.child("polygon");

					if (polyobject == NULL) {
						polyobject = object.child("ellipse");

						if (polyobject == NULL) {
							//ITS A QUAD
							if (object.attribute("id").as_int() == 407) {
								int a = 1;
							}
							SDL_Rect r = { object.attribute("x").as_int() , object.attribute("y").as_int() ,
										   object.attribute("width").as_int() ,object.attribute("height").as_int() };
							if (group_name == "Colliders") {
								App->collision->AddCollider(r, COLLIDER_FLOOR);
							}
							else if (group_name == "Colliders_death")
							{
								App->collision->AddCollider(r, COLLIDER_DEATH);
							}
							else if (group_name == "Colliders_wall")
							{
								App->collision->AddCollider(r, COLLIDER_WALL);
							}
							else if (group_name == "Colliders_floor")
							{
								App->collision->AddCollider(r, COLLIDER_FLOOR);
							}
							else if (group_name == "Collider_start") {
								App->collision->AddCollider(r, COLLIDER_START);
							}
							else if (group_name == "Collider_end") {
								App->collision->AddCollider(r, COLLIDER_END);
							}
							continue;
						}
						//ellipse

					}
				}
				PolyLine* p = new PolyLine();
				iPoint start = { object.attribute("x").as_int(), object.attribute("y").as_int() };
				p->start = start;
				//LOG("id: %d [ %d,%d ]", object.attribute("id").as_int(), start.x, start.y);

				p2SString s(polyobject.attribute("points").as_string());


				//Deal with the string to find each point
				const char* c = polyobject.attribute("points").as_string();
				char* buffer = new char();

				iPoint ret;
				int j = 0;
				for (uint i = 0; i <= strlen(c); i++) {

					char k = c[i];
					if (k == ',') {
						ret.x = atoi(buffer);
						//LOG("x: %d , ", ret.x);
						while (j > 0) {
							buffer[--j] = 0;
						}
					}
					else if (k == ' ' || k == '\0') {
						ret.y = atoi(buffer);
						//LOG("y: %d , ", ret.y);
						while (j > 0) {
							buffer[--j] = 0;
						}
						p->points.add(ret);					// Had finished parsing one point, time to add it
					}
					else {
						buffer[j++] = c[i];
					}
				}
				App->collision->polylines.add(p);
				App->collision->n_lines += 1;

				// END DEALING OBJECT NODE
			}
		}
		//LOG("AAAAAAAAAAAAAAAAAA: %d", a);
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
			LOG("Tileset ----");
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
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
		item_layer->~p2List_item();
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
	LOG("Unloading map");

	// Remove all tilesets /////////////////
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

	while(tileset != NULL)
	{
		App->tex->UnLoad(tileset->data->texture);
		RELEASE(tileset->data);
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
		RELEASE(layer->data);
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
		RELEASE(imagelayer->data);
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
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
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

p2SString j1Map::DebugToString() const
{
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_pos = WorldToMap(x, y);
	
	//int map_id  = data.map_layers.start->data->GetMapId(map_pos.x, map_pos.y);
	//int tile_id = data.map_layers.start->data->data[map_id + abs(App->render->camera.x/data.tile_width)];

	// Loading info to title FLASHES WINDOW ICON IN TASK BAR
	p2SString ret_string("Map: %dx%d Tiles: %dx%d Tilesets: %d Mouse [%d,%d] Rect [%d,%d] Camera.x: %d offsetX: %d",
		data.width, data.height,
		data.tile_width, data.tile_height,
		data.tilesets.count(),
		x, y,
		map_pos.x,map_pos.y, 
		//map_id,tile_id, MapID: %d TilesetID: %d
		App->render->camera.x,
		(data.tile_width > 0) ? abs(App->render->camera.x / data.tile_width): -5000);

	return ret_string;
}

void j1Map::CleanMap()
{
	LOG("Unloading map");

	// Remove all tilesets /////////////////
	p2List_item<TileSet*>* tileset;
	tileset = data.tilesets.start;

	while (tileset != NULL)
	{
		App->tex->UnLoad(tileset->data->texture);
		RELEASE(tileset->data);
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
		RELEASE(layer->data);
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
		RELEASE(imagelayer->data);
		imagelayer = imagelayer->next;
	}
	data.image_layers.clear();
	imagelayer->~p2List_item();

	//Removes all colliders
	if (App->collision->active) {
		App->collision->CleanColliders();
		App->collision->CleanPolylines();
	}
}
