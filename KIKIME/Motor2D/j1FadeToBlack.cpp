#include <math.h>
#include "p2Log.h"
#include "j1App.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Window.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

j1FadeToBlack::j1FadeToBlack()
{
	name.create("fade");
}

j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	BROFILER_CATEGORY("Fade->Start", Profiler::Color::Black)
	LOG("Preparing Fade Screen");
	uint w, h;
	App->win->GetWindowSize(w, h);
	screen = { 0, 0,  int(w * App->win->GetScale()), int(h * App->win->GetScale()) };
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	BROFILER_CATEGORY("Fade->Update", Profiler::Color::Black)
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			if (to_enable != nullptr && to_disable != nullptr)
			{
				to_disable->Disable();
				to_enable->Enable();
			}
			current_step = fade_step::wait;
		}
	} break;

	case fade_step::wait:
	{
		if (App->map_loaded) {
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_from_black;
		}
	}
	break;
	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black. At mid point deactivate one j1, then activate the other
bool j1FadeToBlack::FadeToBlack(j1Module* j1_off, j1Module* j1_on, float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		to_enable = j1_on;
		to_disable = j1_off;
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::FakeToBlack(float time)
{
	bool ret = false;

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}

bool j1FadeToBlack::CleanUp() {

	/*delete to_enable;
	delete to_disable;*/

	return true;
}