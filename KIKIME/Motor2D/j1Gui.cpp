#include "p2Defs.h"
#include "p2Log.h"
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
  
	//HARDCODED
	//CreateElement(IMAGE, iPoint(App->render->viewport.w / 2, App->render->viewport.h / 11 + 25));

	CreateElement(BUTTON, iPoint(10, 600), SDL_Rect({ 641,166,228,68 }), nullptr, MAIN_MENU, (j1Module*)App);
	CreateElement(LABEL, iPoint(90, 625), { 0,0,0,0 }, "MAIN MENU", INFO);

	CreateElement(BUTTON, iPoint(780, 600), SDL_Rect({ 641,166,228,68 }), nullptr, PLAY_PAUSE, (j1Module*)App);
	CreateElement(LABEL, iPoint(855, 625), { 0,0,0,0 }, "PAUSE & PLAY", INFO);

	CreateElement(IMAGE, iPoint(10,10), App->entitymanager->heart.idle.GetCurrentFrame(), nullptr, LIFE_SYSTEM);

	CreateElement(IMAGE, iPoint(700, 10), App->entitymanager->green_diamond.idle.GetCurrentFrame(), nullptr, INFO);
	CreateElement(LABEL, iPoint(745, 15), { 0,0,0,0 }, "X 50", INFO);
	
	CreateElement(IMAGE, iPoint(800, 10), App->entitymanager->blue_diamond.idle.GetCurrentFrame(), nullptr, INFO);
	CreateElement(LABEL, iPoint(845, 15), { 0,0,0,0 }, "X 100", INFO);


	CreateElement(LABEL, iPoint(900, 15), { 0,0,0,0 }, "SCORE : 9999", SCORE);
	CreateElement(LABEL, iPoint(900, 40), { 0,0,0,0 }, "", GAME_TIMER);

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		item->data->PreUpdate();

		HandleInput(item->data);

		item = item->next;
	}

	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	p2List_item<UIElement*>* item = elements.start;

	while (item != NULL)
	{
		item->data->PostUpdate();
		item->data->Draw(atlas);

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

UIElement* j1Gui::CreateElement(UIType type, iPoint pos, SDL_Rect rect, p2SString string, ActionType action, j1Module* callback)
{
	UIElement* elem = nullptr;

	switch (type)
	{
	case IMAGE:

		elem = new Image(pos, rect, type, action);
		break;
	case LABEL:

		elem = new Label(action, pos, type, string);
		break;
	case BUTTON:

		//elem = new Button(action, pos, SDL_Rect({ 641,166,228,68 }), type, callback);
		elem = new Button(action, pos, rect, type, callback);

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

	bool is_inside = (mouse.x - App->render->camera.x >= element->position.x && mouse.x - App->render->camera.x <= element->position.x + element->rect[element->state].w &&
					  mouse.y - App->render->camera.y >= element->position.y && mouse.y - App->render->camera.y <= element->position.y + element->rect[element->state].h);

	bool is_changing = false;
	UIState prev_state = element->state;
	
	if (element->state != CLICK_DOWN &&	is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_DOWN)
	{
		element->state = HOVER;
		//LOG("hover");
	}
	else if((element->state == HOVER || element->state == CLICK_DOWN) && is_inside && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN))
	{
		element->state = CLICK_DOWN;
		//LOG("click");
	}
	else if (is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		element->state = CLICK_UP;
		//LOG("click up");
	}
	else
	{
		element->state = IDLE;
		//LOG("idle");
	}

    is_changing = (prev_state != element->state);

	switch (element->type)
	{
	case BUTTON:
		if(element->state == CLICK_UP)
			element->HandleAction();
		break;
	case LABEL:
		if (is_changing)
			element->HandleAction();
		break;
	case IMAGE:
		break;
	}

}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

// class Gui ---------------------------------------------------

