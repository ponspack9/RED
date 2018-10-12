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

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
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
	//img = App->tex->Load("textures/test.png");
	//TODO Load from audio folders not hardcoded, see example below
	bool ret = App->audio->PlayMusic("audio/music/music_sadpiano.ogg");
	if (ret) App->map->Load(App->map->current_map->data.GetString());
	

	//Two debug colliders,one at the beginning of the map and at the end
	App->collision->AddCollider(SDL_Rect({ 0,0,32,32 }), COLLIDER_GROUND);
	//App->collision->AddCollider(SDL_Rect({ 0,0,32,32 }), COLLIDER_PLAYER);

	int real_w = App->map->data.width *32;
	int real_h = App->map->data.height *32;
	//App->map->MapToWorld(real_w,real_h);
	App->collision->AddCollider(SDL_Rect({ real_w-32,real_h-32,32,32 }), COLLIDER_GROUND);

	int a = 1;
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
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += 1;

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= 1;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += 1;

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= 1;

	App->map->Draw();
	

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
	App->map->CleanUp();
	return true;
}

bool j1Scene::Save(pugi::xml_node & node)
{
	LOG("Saving SCENE");

	pugi::xml_node map_node = node.append_child("current");
		
	map_node.append_attribute("current_map") = App->map->current_map->data.GetString();
	
	LOG("current map %s - %s", App->map->current_map->data.GetString(), node.child("scene").child("current").attribute("current_map").as_string());
	return true;
}

bool j1Scene::Load(pugi::xml_node & node)
{
	LOG("Loading SCENE");

	App->map->current_map->data = node.child("scene").child("current").attribute("current_map").as_string();

	return true;
}
