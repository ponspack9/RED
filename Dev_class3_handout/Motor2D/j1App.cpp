#include <iostream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1App.h"

// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	frames = 0;
	want_to_save = want_to_load = false;

	input	= new j1Input();
	win		= new j1Window();
	render	= new j1Render();
	tex		= new j1Textures();
	audio	= new j1Audio();
	scene	= new j1Scene();
	map	= new j1Map();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	// RENDER last to swap buffer
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(audio);
	AddModule(map);
	AddModule(scene);

	AddModule(render);
}

// Destructor
j1App::~j1App()
{
	// release modules
	p2List_item<j1Module*>* item = modules.end;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->prev;
	}

	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.add(module);
}

// Called before render is available
bool j1App::Awake()
{
	bool ret = true;

		// BEGIN CONFIGURATION
	pugi::xml_document	config_doc;
	pugi::xml_node		config;
	pugi::xml_node		app_config;
	pugi::xml_node		file_system;

	pugi::xml_parse_result result = LoadXML(config_doc, "config.xml");

	if (result != NULL)
	{
		config		= config_doc.child("config");

		app_config	= config.child("app");
		file_system = config.child("file_system");

		title.create(app_config.child("title").child_value());
		organization.create(app_config.child("organization").child_value());

		load_path.create(file_system.child("load_path").child_value());
		save_path.create(file_system.child("save_path").child_value());
		LOG("LoadPath: %s", load_path.GetString());
	
		// END CONFIGURATION

		// Awakening all modules
		p2List_item<j1Module*>* item = modules.start;

		while(item != NULL && ret)
		{
			ret = item->data->Awake(config.child(item->data->name.GetString()));
			item = item->next;
		}
	}

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.start;

	while(item != NULL && ret)
	{
		ret = item->data->Start();
		item = item->next;
	}

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT))
		ret = false;

	if(ret)
		ret = PreUpdate();

	if(ret)
		ret = DoUpdate();

	if(ret)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

pugi::xml_parse_result j1App::LoadXML(pugi::xml_document& doc, const char* path)
{
	pugi::xml_parse_result result = doc.load_file(path);

	if (result == NULL)
	{
		LOG("Could not load xml file %s pugi error: %s",path, result.description());
	}
	else {
		LOG("%s loaded successfully", path);
	}

	return result;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if (want_to_save) SaveGameFile();
	if (want_to_load) LoadGameFile();
}

bool j1App::SaveGameFile() 
{
	LOG("Saving new Game State to %s...", save_path.GetString());

	pugi::xml_document		save_game_doc; 
	pugi::xml_node			save_node;
	p2List_item<j1Module*>* item = modules.start;

	bool ret = true;

	save_node = save_game_doc.append_child("save");

	while (item != NULL && ret)
	{
		if (item->data->active) {
			ret = item->data->Save(save_node.append_child(item->data->name.GetString()));
		}
		item = item->next;
	}

	if (ret) {
		save_game_doc.save_file(save_path.GetString());
		LOG("...finished saving");
	}
	else
		LOG("...saving process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown -> NULL pointer");
	
	save_game_doc.reset();
	want_to_save = false;
	return ret;
}

bool j1App::LoadGameFile()
{
	pugi::xml_document		save_game_doc;
	pugi::xml_node			save_node;
	pugi::xml_parse_result	result = LoadXML(save_game_doc, load_path.GetString());

	bool ret = result != NULL;

	p2List_item<j1Module*>* item = modules.start;

	if (ret) {
		LOG("Loading new Game State from %s...", load_path.GetString());

		save_node = save_game_doc.child("save");

		while (item != NULL && ret)
		{
			if (item->data->active) {
				ret = item->data->Load(save_node.child(item->data->name.GetString()));
			}
			item = item->next;
		}

		if (ret)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->data->name.GetString() : "unknown -> NULL pointer");
	}

	want_to_load = false;
	return ret;
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.start;
	
	while (item != NULL && ret)
	{
		if (item->data->active) {
			ret = item->data->PreUpdate();
		}
		item = item->next;
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.start;

	while (item != NULL && ret)
	{
		if (item->data->active) {
			ret = item->data->Update(dt);
		}
		item = item->next;
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.start;
	
	while (item != NULL && ret)
	{
		if (item->data->active) {
			ret = item->data->PostUpdate();
		}
		item = item->next;
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	bool ret = true;
	p2List_item<j1Module*>* item = modules.end;
	
	while(item != NULL && ret)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.GetString();
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.GetString();
}

void j1App::LoadGame()
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
}

void j1App::SaveGame() const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
}

void j1App::GetSaveGames(p2List<p2SString>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}


