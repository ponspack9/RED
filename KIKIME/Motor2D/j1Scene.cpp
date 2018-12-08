#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "Player.h"
#include "j1FadeToBlack.h"
#include "j1EntityManager.h"
#include "j1Pathfinding.h"
#include "Entity.h"
#include "j1Debug.h"

j1Scene::j1Scene() : j1Module() 
{
	name.create("scene");
	first_load = true;
	game_over_transition = false;
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{ 
	
	LOG("Loading Scene");

	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	bool ret = true;
	App->game_over = false;

	// App->audio->PlayMusic(PATH(App->audio->folder_music.GetString(), App->audio->tracks_path.start->data.GetString()));
	App->map->Load(App->map->current_map->data.GetString());

	App->render->ResetCamera();

	if (first_load) {
		first_load = false;
	}
	else {
		App->entitymanager->Restart();
		LOG("ENTITY RESTART");
	}
	//LOG("CURRENTMAP FROM SCENE: %s", App->map->current_map->data.GetString());
	// Game Over transition to main menu
	if (strcmp(App->map->current_map->data.GetString(), App->map->maps_path.end->prev->data.GetString()) == 0 && !game_over_transition) {
		LOG("GAMEOVER MAP LOADED");
		game_over_timer.Start();
		game_over_transition = true;
	}

	return ret;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}
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

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->MoveCamera(0, 1);

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->MoveCamera(0, -1);

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->MoveCamera(1, 0);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->MoveCamera(-1, 0);

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		App->entitymanager->CreateEntity(FLOATER, p);

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		App->entitymanager->CreateEntity(ROLLER, p);

	App->map->Draw();
	// Shows up player in main menu
	//if (!game_over_transition && App->fade->current_step == App->fade->fade_step::none)
	//No showing entities till playing (player pops up an instant before)
	if (strcmp(App->map->current_map->data.GetString(), App->map->maps_path.end->prev->data.GetString()) != 0 &&
		strcmp(App->map->current_map->data.GetString(), App->map->maps_path.end->data.GetString()) != 0 /*&&
		App->fade->current_step == App->fade->fade_step::none*/)
	App->entitymanager->Draw();

	if (game_over_timer.ReadSec() > 2.5f && game_over_transition && App->fade->current_step == App->fade->fade_step::none) {
		LOG("TRANSITIONING TO MAIN MENU");
		App->GoToMainMenu();
		game_over_transition = false;
	}
	// Debug pathfinding ------------------------------
	//int x, y;
	//App->input->GetMousePosition(x, y);

	//App->render->Blit(debug_tex, p.x, p.y);
	//
	//const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();
	//
	//for (uint i = 0; i < path->Count(); ++i)
	//{
	//	iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
	//	App->render->Blit(debug_tex, pos.x, pos.y);
	//}
	

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
	//App->map->CleanUp();
	App->map->CleanMap();

	return true;
}

bool j1Scene::Save(pugi::xml_node & node)
{
	LOG("Saving SCENE");

	pugi::xml_node map_node = node.append_child("current");
		
	map_node.append_attribute("current_map") = App->map->current_map->data.GetString();
	
	LOG("current map %s - %s", App->map->current_map->data.GetString(), node.child("current").attribute("current_map").as_string());
	return true;
}

bool j1Scene::Load(pugi::xml_node & node)
{
	LOG("Loading SCENE");
	   

	App->map->current_map->data = node.child("current").attribute("current_map").as_string();


	LOG("Current map: %s", App->map->current_map->data.GetString());

	return true;
}
