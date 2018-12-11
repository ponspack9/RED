#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "Player.h"
#include "j1Map.h"
#include "j1EntityManager.h"

#define VSYNC false

j1Render::j1Render() : j1Module()
{
	name.create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	//BROFILER_CATEGORY("Render->Awake", Profiler::Color::Beige)
	LOG("Create SDL rendering context");
	bool ret = true;

	// load flags
	uint32 flags = SDL_RENDERER_ACCELERATED;

	if (config.child("vsync").attribute("value").as_bool() == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}
	else
	{
		LOG("Not Using vsync");
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);

	if (renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}

	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	BROFILER_CATEGORY("Render->Start", Profiler::Color::Beige)
	LOG("render start");
	// back background
	//SDL_RenderSetLogicalSize(App->render->renderer, App->win->width, App->win->height);
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	BROFILER_CATEGORY("Render->PreUpdate", Profiler::Color::Beige)
	SDL_RenderClear(renderer);
	SDL_RenderGetViewport(renderer, &viewport);
	
	return true;
}

bool j1Render::Update(float dt)
{
	
	return true;
}

bool j1Render::PostUpdate()
{
	BROFILER_CATEGORY("Render->PostUpdate", Profiler::Color::Beige)
	if (!App->debug->free_camera)FollowPlayer();
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	BROFILER_CATEGORY("Render->CleanUp", Profiler::Color::Beige)
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

bool j1Render::Load(pugi::xml_node& node)
{

	LOG("Loading RENDER");

	camera.x = node.child("camera").attribute("x").as_int();
	camera.y = node.child("camera").attribute("y").as_int();

	return true;
}
bool j1Render::Save(pugi::xml_node& node)
{

	LOG("Saving RENDER");
	pugi::xml_node cam = node.append_child("camera");

	cam.append_attribute("x") = camera.x;
	cam.append_attribute("y") = camera.y;
	LOG("cameraX: %d - %d \n camerY: %d - %d", camera.x, cam.attribute("x").as_int(), camera.y, cam.attribute("y").as_int());

	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

bool j1Render::MoveCamera(const int & vel_x, const int & vel_y)
{
	SDL_RenderGetViewport(renderer, &viewport);

	int x = App->map->world_limits.x - viewport.w;
	int y = App->map->world_limits.y - viewport.h;

	if (abs(camera.x + vel_x) < x && camera.x + vel_x < 0) 
	{
	//Camera can move x axis
		camera.x += vel_x;
	
		if (abs(camera.y + vel_y) < y && camera.y + vel_y < 0) {
			// Camera can move both axis
			camera.y += vel_y;
			return true;
		}
		return true;

	}
	else if (abs(camera.y + vel_y) < y && camera.y + vel_y < 0) 
	{
	//Camera can move y axis
		camera.y += vel_y;
		return true;
	}

	return false;
}

void j1Render::FollowPlayer()
{
	BROFILER_CATEGORY("Render->FollowCamera", Profiler::Color::Beige)
	camera.x = -App->entitymanager->player_ref->position.x + camera.w / 3;
	camera.y = -App->entitymanager->player_ref->position.y + camera.h / 2;

	if (camera.x > 0) //left limit
	{
		camera.x = 0;
	}
	if (camera.x - camera.w < -App->map->data.width * App->map->data.tile_width) //right limit
	{
		camera.x = -App->map->data.width * App->map->data.tile_width + camera.w;
	}
	if (camera.y > 0) //top limit
	{
		camera.y = 0;
	}
	if (camera.y - camera.h < -App->map->data.height * App->map->data.tile_height) //down limit
	{
		camera.y = -App->map->data.height * App->map->data.tile_height + camera.h;
	}
}

void j1Render::ResetCamera()
{
	camera.x = 0;
	camera.y = 0;
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}
iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();

	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}
// Blit to screen
bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, double angle, SDL_RendererFlip flip, int pivot_x, int pivot_y, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();
	SDL_Rect rect;

	if (use_camera) {
		rect.x = (int)(camera.x * speed) + x * scale;
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	else {
		rect.x = (int) x * scale;
		rect.y = (int) y * scale;
	}
	

	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if (use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for (uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x + radius * cos(i * factor));
		points[i].y = (int)(y + radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}