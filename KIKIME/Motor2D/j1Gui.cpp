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
	CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 , App->render->viewport.h / 12 + 25));

	CreateElement(BUTTON, iPoint(50, 50));
	//CreateElement(IMAGE, iPoint(App->render->viewport.w /2 , App->render->viewport.h / 12  + 100));

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	HandleInput();

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	p2List_item<UIElement*>* item = elements.start;

	int w, h;

	SDL_Texture* text = App->font->Print("HELLO WORLD", { 255,0,0,255 }, App->font->default);
	App->font->CalcSize("HELLO WORLD", w, h, App->font->default);

	App->render->Blit(text, App->render->viewport.w / 2 - App->render->camera.x-w/2, App->render->viewport.h / 12 - App->render->camera.y);

	while (item != NULL)
	{
		App->render->Blit(atlas, item->data->position.x - App->render->camera.x, item->data->position.y - App->render->camera.y, &item->data->rect[item->data->state]);

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

void j1Gui::HandleInput()
{
	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		if (item->data->type == BUTTON)
		{
			iPoint mouse;
			App->input->GetMousePosition(mouse.x, mouse.y);

			LOG("mouse pos : %d - %d", mouse.x, mouse.y);
			LOG("button rect: %d - %d - %d - %d", item->data->position.x, item->data->position.y, item->data->rect[item->data->state].w, item->data->rect[item->data->state].h);
			
			if (mouse.x > item->data->position.x && mouse.x < item->data->position.x + item->data->rect[item->data->state].w &&
				mouse.y > item->data->position.y &&	mouse.y < item->data->position.y + item->data->rect[item->data->state].h &&
				App->input->GetMouseButtonDown(SDL_SCANCODE_LEFT) == KEY_UP)
			{
				item->data->state = HOVER;
				LOG("hover");
			}
			else if (mouse.x > item->data->position.x && mouse.x < item->data->position.x + item->data->rect[item->data->state].w &&
					 mouse.y > item->data->position.y && mouse.y < item->data->position.y + item->data->rect[item->data->state].h &&
				     App->input->GetMouseButtonDown(SDL_SCANCODE_LEFT) == KEY_DOWN)
			{
				item->data->state = CLICK_DOWN;
				LOG("click");
			}
			else
			{
				item->data->state = IDLE;
				LOG("idle");
			}
		}


		item = item->next;
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

