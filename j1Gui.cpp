#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "j1Window.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{

	atlas = App->tex->Load(atlas_file_name.GetString());
	SDL_RenderGetViewport(App->render->renderer, &App->render->viewport);
	CreateElement(PICTURE, iPoint(App->render->viewport.w / 2 , App->render->viewport.h / 12 + 25));
	CreateElement(PICTURE, iPoint(App->render->viewport.w /2 , App->render->viewport.h / 12  + 100));

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	p2List_item<UIElements>* item = elements.start;

	int w, h;

	SDL_Texture* text = App->font->Print("HELLO WORLD", { 255,0,0,255 }, App->font->default);
	App->font->CalcSize("HELLO WORLD", w, h, App->font->default);

	App->render->Blit(text, App->render->viewport.w / 2 - App->render->camera.x-w/2, App->render->viewport.h / 12 - App->render->camera.y);

	while (item != NULL)
	{
		App->render->Blit(atlas, item->data.pos.x - App->render->camera.x - item->data.rect.w/2, item->data.pos.y - App->render->camera.y, &item->data.rect);

		item = item->next;
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	p2List_item<UIElements>* item = elements.start;

	/*while (item != NULL)
	{
		RELEASE(item->data);		
		item = item->next;
	}*/

	return true;
}

UIElements * j1Gui::CreateElement(UIType type, iPoint pos)
{
	UIElements elem; // = new UIElements();

	switch (type)
	{
	case PICTURE:
		elem.rect = SDL_Rect({ 485,829,328,103 });
		break;
	case LABEL:

		break;
	default:
		elem.rect = { 0,0,0,0 };
	}

	elem.pos = pos;
	elem.type = type;
	elements.add(elem);

	return &elem;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

