#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Audio.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1EntityManager.h"
#include "j1Gui.h"

j1Scene::j1Scene() : j1Module() 
{
	name.create("scene");
	first_load = true;
	game_over_transition = false;
	player_load_position = { -1,-1 };
	//is_load = false;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{ 	
	LOG("Loading Scene");
	current_track = App->audio->tracks_path.start;
	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;
	App->game_over = false;

	App->audio->PlayMusic(PATH(App->audio->folder_music.GetString(), current_track->data.GetString()));
	LOG("starting scene");

	App->map->Load(App->map->current_map->data.GetString());
	App->render->ResetCamera();

	if (first_load) {
		first_load = false;
	}
	else {
		//if (!is_load)
		App->entitymanager->Restart(App->entitymanager->playerinfo.lifes,player_load_position);
		player_load_position = { -1,-1 };
		App->gui->game_over->SetInvisible();

		LOG("ENTITY RESTART");
		App->gui->last_death->position = { -100,-100 };
		App->player_in_main_menu = strcmp(App->map->current_map->data.GetString(), App->map->maps_path.start->data.GetString()) == 0;
		if (App->player_in_main_menu) {
			App->gui->PrepareMainMenuGui();
		}
		else {
			App->gui->PrepareInGameGui();
		}
	}
	//is_load = false;
	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	int x;
	int y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->map->Draw();
	App->entitymanager->Draw();

	// Game Over transition to main menu
	if (App->game_over && !game_over_transition) {
		game_over_timer.Start();
		game_over_transition = true;
	}

	if (game_over_timer.ReadSec() > 2.1f && game_over_transition) {
		LOG("TRANSITIONING TO MAIN MENU");
		App->GoToMainMenu();
		App->gui->game_over->visible = false;
		game_over_transition = false;
	}	
	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	return true;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");
	App->map->CleanMap();

	return true;
}

bool j1Scene::Save(pugi::xml_node & node)
{
	LOG("Saving SCENE");

	pugi::xml_node map_node = node.append_child("current");
		
	map_node.append_attribute("current_map") = App->map->current_map->data.GetString();

	pugi::xml_node time_node = node.append_child("timer");
	time_node.append_attribute("seconds") = App->in_game_timer.sec;
	time_node.append_attribute("minutes") = App->in_game_timer.min;
	
	LOG("current map %s - %s", App->map->current_map->data.GetString(), node.child("current").attribute("current_map").as_string());
	return true;
}

bool j1Scene::Load(pugi::xml_node & node)
{
	LOG("Loading SCENE");	 

	//App->map->current_map->data = node.child("current").attribute("current_map").as_string();
	int index = App->map->maps_path.find(node.child("current").attribute("current_map").as_string());

	App->map->current_map = App->map->maps_path.At(index);
	//App->map->CleanMap();
	//App->scene->Start();

	App->in_game_timer.sec = node.child("timer").attribute("seconds").as_int();
	App->in_game_timer.min = node.child("timer").attribute("minutes").as_int();

	//is_load = true;
	App->game_timer.Start();
	LOG("Current map: %s", App->map->current_map->data.GetString());

	return true;
}

