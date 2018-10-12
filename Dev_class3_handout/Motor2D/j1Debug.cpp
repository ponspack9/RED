#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1Scene.h"
#include "j1FadeToBlack.h"
#include "j1Debug.h"



j1Debug::j1Debug()
{
	name.create("debug");
}


j1Debug::~j1Debug()
{
}

bool j1Debug::Awake(pugi::xml_node & config)
{
	show_colliders = true;
	start_motion = false;
	zero.SetToZero();

	return true;
}

bool j1Debug::PostUpdate()
{
	bool ret = true;

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

bool j1Debug::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->RestartGame();
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	{
		App->RestartLevel();
	}
	// Next level
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	{
		if (App->map->current_map->next != NULL)
			App->map->current_map = App->map->current_map->next;
		else
			App->map->current_map = App->map->maps_path.start;

		LOG("Next level: %s", App->map->current_map->data.GetString());
		App->fade->FadeToBlack(App->scene, App->scene);
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		App->SaveGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
	{
		App->LoadGame();
	}

	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		show_colliders = !show_colliders;

	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		//FUCKING GOD MODE CUSI

	}
	//Magic
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
		SDL_RenderDrawLine(App->render->renderer, camera_motion.x, camera_motion.y, final.x, final.y);

		App->input->GetMouseMotion(final.x, final.y);

		last_motion -= final;
		if (last_motion != zero) {
			App->render->camera.x += final.x;
			App->render->camera.y += final.y;

		}

		last_motion = { final.x, final.y };
	}
	

	App->win->SetTitle(App->map->DebugToString().GetString());
	return true;
}
