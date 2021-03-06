#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Collision.h"
#include "j1FadeToBlack.h"
#include "j1Debug.h"
#include "j1EntityManager.h"
#include "j1Gui.h"



j1Debug::j1Debug()
{
	name.create("debug");

	not = nullptr;
	debug_path = nullptr;

	not_tex = nullptr;
	debug_tex = nullptr;

	camera_motion = { 0,0 };
	last_motion = { 0,0 };
	zero = { 0,0 };

	show_colliders = false;
	start_motion = false;
	free_camera = false;
	debug_bool = false;

}


j1Debug::~j1Debug()
{
}

bool j1Debug::Awake(pugi::xml_node & config)
{
	//BROFILER_CATEGORY("Debug->Awake", Profiler::Color::HotPink)

	show_colliders = config.child("showlogic").attribute("value").as_bool();
	show_debug_ui = config.child("showdebugui").attribute("value").as_bool();
	free_camera = config.child("free_camera").attribute("value").as_bool();
	show_fps = config.child("showfps").attribute("value").as_bool();
	debug_path.create(config.child("debugtexture").child_value());
	not.create(config.child("nottexture").child_value());
	
	zero.SetToZero();

	return true;
}

bool j1Debug::Start() {
	not_tex = App->tex->Load(not.GetString());
	debug_tex = App->tex->Load(debug_path.GetString());
	return true;
}
bool j1Debug::PostUpdate()
{
	BROFILER_CATEGORY("Debug->PostUpdate", Profiler::Color::HotPink)
	bool ret = true;

	if (!App->player_in_main_menu)
	{
		if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
			App->TogglePause();
			if (App->gui->settings_window->visible) {
				App->gui->PrepareInGameGui();
			}
			else {
				App->ShowInGamePause();
			}
		}
	}
	return ret;
}

bool j1Debug::Update(float dt)
{
	BROFILER_CATEGORY("Debug->Update", Profiler::Color::HotPink);

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->RestartGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->RestartLevel(App->entitymanager->player_ref->lifes);
	}
	// Next level
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		if (App->fade->current_step == App->fade->fade_step::none)  
			App->NextLevel();
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN)
	{
		free_camera = !free_camera;
	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->SaveGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN)
	{
		show_fps = !show_fps;
	}

	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		show_debug_ui = !show_debug_ui;

	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		show_colliders = !show_colliders;

	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (!App->entitymanager->player_ref->god_mode) {
			App->entitymanager->player_ref->god_mode = true;
			if (App->entitymanager->player_ref->collider) 
				App->entitymanager->player_ref->collider->type = COLLIDER_NONE;
		}
		else {
			App->entitymanager->player_ref->god_mode = false;
			if (App->entitymanager->player_ref->collider) 
				App->entitymanager->player_ref->collider->type = COLLIDER_PLAYER;
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		App->delay_is_active = !App->delay_is_active;
	}
	if (App->input->GetKey(SDL_SCANCODE_F12) == KEY_DOWN)
	{
		//Do not crash
		int a = 1;
	}

	int x;
	int y;
	App->input->GetMousePosition(x, y);
	x -= App->render->camera.x;
	y -= App->render->camera.y;

	//Spawining Entities
	//if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
	//{
	//	Entity* e = App->entitymanager->CreateEntity(COIN, { x,y }, GREEN_DIAMOND);
	//	e->collider = App->collision->AddCollider(e->rect, COLLIDER_COIN);
	//
	//}
	//if (App->input->GetKey(SDL_SCANCODE_B) == KEY_DOWN)
	//{
	//	Entity* e = App->entitymanager->CreateEntity(COIN, { x,y }, BLUE_DIAMOND);
	//	e->collider = App->collision->AddCollider(e->rect, COLLIDER_COIN);
	//
	//}
	//if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN)
	//{
	//	Entity* e = App->entitymanager->CreateEntity(COIN, { x,y },HEART);
	//	e->collider = App->collision->AddCollider(e->rect, COLLIDER_COIN);
	//
	//}
	//if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
	//	Entity* e = App->entitymanager->CreateEntity(FLOATER, { x,y });
	//	e->collider = App->collision->AddCollider(e->rect, COLLIDER_DEATH);
	//}
	//
	//if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
	//	Entity* e = App->entitymanager->CreateEntity(ROLLER, { x,y });
	//	e->collider = App->collision->AddCollider(e->rect, COLLIDER_DEATH);
	//}

	// Camera drag through mouse click
	if (free_camera) {
	
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			LOG("CLICK");
			App->input->GetMousePosition(camera_motion.x, camera_motion.y);
			start_motion = true;
		}

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
			LOG("CLICK UP");
			start_motion = false;
			//App->input->GetMousePosition(camera_motion.x, camera_motion.y);
		}
		if (start_motion) {

			iPoint final;
			App->input->GetMousePosition(final.x, final.y);
			SDL_SetRenderDrawColor(App->render->renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
			if (show_colliders)SDL_RenderDrawLine(App->render->renderer, camera_motion.x, camera_motion.y, final.x, final.y);

			App->input->GetMouseMotion(final.x, final.y);
			LOG("final_motio: %d,%d", final.x, final.y);
			LOG("last_motion: %d,%d", last_motion.x, last_motion.y);

			last_motion -= final;
			LOG("minus_motio: %d,%d", last_motion.x, last_motion.y);

			if (last_motion != zero) {
				App->render->MoveCamera(final.x, final.y);
			}

			last_motion = { final.x, final.y };
		}
	}
	
	return true;
}
