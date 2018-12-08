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

	//CreateElement(BUTTON, iPoint(App->render->viewport.w / 12, App->render->viewport.h / 12), SDL_Rect({ 641,166,228,68 }), nullptr, MAIN_MENU, (j1Module*)App);
	////CreateElement(BUTTON, iPoint(5 * App->render->viewport.w / 12, 6 * App->render->viewport.h / 12), nullptr, SETTINGS);
	// 
	//CreateElement(IMAGE, iPoint(10,10), SDL_Rect({ 997,706,18,18 }), nullptr, NO_ACTION, (j1Module*)App);
	//CreateElement(IMAGE, iPoint(10,40), SDL_Rect({ 1001,928,18,18 }), nullptr, NO_ACTION, (j1Module*)App);
	//
	//CreateElement(LABEL, iPoint(20, 20), { 0,0,0,0 }, "0", GAME_TIMER);
	//CreateElement(LABEL, iPoint(9 * App->render->viewport.w / 10, 20), { 0,0,0,0 }, "SCORE : 999", SCORE);

	in_game_pause = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w/2 - 214, App->render->viewport.h/2 - 226),SDL_Rect({ 28,542,428,452 }), nullptr, NO_ACTION, (j1Module*)App,nullptr,false);
	Button* b1 = (Button*)CreateElement(BUTTON, iPoint(0, 20), SDL_Rect({ 641,166,228,68 }), nullptr, PLAY_PAUSE, nullptr,  in_game_pause);
	Button* b2 = (Button*)CreateElement(BUTTON, iPoint(40, 90), SDL_Rect({ 641,166,228,68 }), nullptr, SETTINGS,nullptr,  in_game_pause);
	Button* b3 = (Button*)CreateElement(BUTTON, iPoint(40, 160), SDL_Rect({ 641,166,228,68 }), nullptr, EXIT_GAME, nullptr, in_game_pause);
	b1->CenterX(); b1->CenterY(-70);
	b2->CenterX(); b2->CenterY(0);
	//b3->CenterX(); b3->CenterY(70);
	b3->Center(0, 70);

	Label* l1 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "RESUME GAME", NO_ACTION, nullptr, b1);
	Label* l2 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "SETTINGS", NO_ACTION, nullptr, b2);
	Label* l3 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "EXIT GAME", NO_ACTION,nullptr,b3);
	l1->CenterX();	l1->CenterY();
	l2->CenterX();	l2->CenterY();
	l3->CenterX();	l3->CenterY();

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		if (item->data->visible) {

			item->data->PreUpdate();
			HandleInput(item->data);

		}

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
		if (item->data->visible) {

			if (item->data->type == UIType::LABEL) {
				Label* l = ((Label*)item->data);
				l->Draw(l->text);
			}
			else {
				item->data->Draw(atlas);
			}

		}
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
		delete item->data;

		item = item->next;
	}
	
	return true;
}

UIElement* j1Gui::CreateElement(UIType type, iPoint pos, SDL_Rect rect, p2SString string, ActionType action, j1Module* callback, UIElement* parent, bool visible)
{
	UIElement* elem = nullptr;

	switch (type)
	{
	case IMAGE:

		elem = new Image(pos, rect, type, parent,visible);
		break;
	case LABEL:

		elem = new Label(action, pos, type, string, parent, visible);
		break;
	case BUTTON:

		elem = new Button(action, pos, rect, type, callback, parent, visible);
		break;
	default:

		LOG("COULD NOT IDENTIFY UI ELEMENT TYPE");
		break;
	}

	elements.add(elem);

	return elem;
}

void j1Gui::HandleInput(UIElement* element)
{
	bool ret = true;
	iPoint mouse;
	App->input->GetMousePosition(mouse.x, mouse.y);

	/*bool is_inside = (mouse.x - App->render->camera.x >= element->position.x && mouse.x - App->render->camera.x <= element->position.x + element->rect[element->state].w &&
					  mouse.y - App->render->camera.y >= element->position.y && mouse.y - App->render->camera.y <= element->position.y + element->rect[element->state].h);
	*/
	bool is_inside = (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
					  mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h);

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
	//Not a nice mouse movement but working for debug for now
	//LOG("ELEMENT [%d,%d] MOUSE [%d,%d]", element->position.x, element->position.y, mouse.x, mouse.y);
	/*if (element->state == CLICK_DOWN) {
		element->position.x = (mouse.x -  100);
		element->position.y = (mouse.y -  100);
	}*/

}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}


