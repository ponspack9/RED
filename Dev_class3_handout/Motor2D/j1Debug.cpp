#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Debug.h"



j1Debug::j1Debug()
{
}


j1Debug::~j1Debug()
{
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
		//App->RestartGame();


	App->win->SetTitle(App->map->DebugToString().GetString());
	return true;
}
