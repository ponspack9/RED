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
	CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 , App->render->viewport.h / 11 + 25));

	CreateElement(BUTTON, iPoint(App->render->viewport.w / 12, App->render->viewport.h / 12));

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		item->data->PreUpdate();

		if (item->data->type == BUTTON)
			HandleInput(item->data);

		item = item->next;
	}

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	int w, h;

	SDL_Texture* text = App->font->Print("HELLO WORLD", { 255,0,0,255 }, App->font->default);
	App->font->CalcSize("HELLO WORLD", w, h, App->font->default);

	App->render->Blit(text, App->render->viewport.w / 2 - App->render->camera.x-w/2, App->render->viewport.h / 12 - App->render->camera.y);

	p2List_item<UIElement*>* item = elements.start;

	while (item != NULL)
	{
		item->data->PostUpdate();
		item->data->Draw(atlas);
		//App->render->Blit(atlas, item->data->position.x - App->render->camera.x, item->data->position.y - App->render->camera.y, &item->data->rect[item->data->state]);

		item = item->next;
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		delete &item->data;

		item = item->next;
	}
	
	return true;
}

UIElement* j1Gui::CreateElement(UIType type, iPoint pos)
{
	UIElement* elem = nullptr;

	switch (type)
	{
	case IMAGE:

		elem = new Image(pos, SDL_Rect({ 485,829,328,103 }), type);
		break;
	case LABEL:

		break;
	case BUTTON:

		elem = new Button(pos, SDL_Rect({648,173,217,56}), type);
		break;
	default:

		break;
	}

	elements.add(elem);

	return elem;
}

void j1Gui::HandleInput(UIElement* element)
{
	iPoint mouse;
	App->input->GetMousePosition(mouse.x, mouse.y);

	LOG("mouse pos : %d - %d", mouse.x, mouse.y);
	LOG("button rect: %d - %d - %d - %d", element->position.x, element->position.y, element->rect[element->state].w, element->rect[element->state].h);
	
	if (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
		mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h &&
		App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_DOWN)
	{
		element->state = HOVER;
		LOG("hover");
	}
	else if (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
			 mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h && 
			 (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN))
	{
		element->state = CLICK_DOWN;
		LOG("click");
	}
	else if (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
			 mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h &&
			 App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		element->state = CLICK_UP;
		LOG("click up");
	}
	else
	{
		element->state = IDLE;
		LOG("idle");
	}
}

void j1Gui::HandleAction()
{
}

void j1Gui::Draw()
{
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

