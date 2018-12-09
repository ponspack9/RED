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
	LOG("Loading GUI");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	blue_file_name = conf.child("blue").attribute("file").as_string("");
	red_file_name = conf.child("red").attribute("file").as_string();
	green_file_name = conf.child("green").attribute("file").as_string();
	yellow_file_name = conf.child("yellow").attribute("file").as_string();
	grey_file_name = conf.child("grey").attribute("file").as_string();

	//Creating game over image
	pugi::xml_node n = conf.child("atlas").child("gameOver");
	for (SDL_Rect &r : game_over_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("gameOver"))
			n = n.next_sibling("gameOver");
	}

	//Creating last death image
	n = conf.child("atlas").child("lastDeath");
	for (SDL_Rect &r : last_death_image.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("lastDeath"))
			n = n.next_sibling("lastDeath");
	}

	/*last_death_image.action = NO_ACTION;
	last_death_image.callback = nullptr;
	last_death_image.initial_pos = { 0,0 };
	last_death_image.position = { 0,0 };
	last_death_image.parent = nullptr;
	last_death_image.state = IDLE;
	last_death_image.type = IMAGE;
	last_death_image.string = nullptr;
	last_death_image.visible = true;*/


	//Creating a blue button
	n = conf.child("blue").child("button");
	for (SDL_Rect &r : blue_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	blue_button.type	 = UIType::BUTTON;
	blue_button.action	 = NO_ACTION;
	blue_button.parent	 = nullptr;
	blue_button.callback = nullptr;
	blue_button.visible	 = true;
	blue_button.color	 = BLUE;


	// Creating a red button
	n = conf.child("red").child("button");
	for (SDL_Rect &r : red_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	red_button.type		= UIType::BUTTON;
	red_button.action	= NO_ACTION;
	red_button.parent	= nullptr;
	red_button.callback = nullptr;
	red_button.visible	= true;
	red_button.color	= RED;

	// Creating a green button
	n = conf.child("green").child("button");
	for (SDL_Rect &r : green_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	green_button.type = UIType::BUTTON;
	green_button.action = NO_ACTION;
	green_button.parent = nullptr;
	green_button.callback = nullptr;
	green_button.visible = true;
	green_button.color = GREEN;

	// Creating a yellow button
	n = conf.child("yellow").child("button");
	for (SDL_Rect &r : yellow_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	yellow_button.type = UIType::BUTTON;
	yellow_button.action = NO_ACTION;
	yellow_button.parent = nullptr;
	yellow_button.callback = nullptr;
	yellow_button.visible = true;
	yellow_button.color = YELLOW;

	// Creating a grey button
	n = conf.child("grey").child("button");
	for (SDL_Rect &r : grey_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		n = n.next_sibling("button");
	}
	grey_button.type = UIType::BUTTON;
	grey_button.action = NO_ACTION;
	grey_button.parent = nullptr;
	grey_button.callback = nullptr;
	grey_button.visible = true;
	grey_button.color = GREY;


	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());
	blue = App->tex->Load(blue_file_name.GetString());
	red = App->tex->Load(red_file_name.GetString());
	green = App->tex->Load(green_file_name.GetString());
	yellow = App->tex->Load(yellow_file_name.GetString());
	grey = App->tex->Load(grey_file_name.GetString());

	SDL_RenderGetViewport(App->render->renderer, &App->render->viewport);
  
	//HARDCODED
	/////////////MAIN MENU //////////////////
	main_menu_ui = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 214, App->render->viewport.h / 2 - 226), SDL_Rect({ 28,542,428,452 }), nullptr, NO_ACTION, (j1Module*)App, nullptr, false);
	Button* m1 = (Button*)CreateElement(BUTTON, iPoint(0, 20), SDL_Rect({ 641,166,228,68 }), nullptr, START, nullptr, main_menu_ui);
	Button* m2 = (Button*)CreateElement(BUTTON, iPoint(40, 90), SDL_Rect({ 641,166,228,68 }), nullptr, SETTINGS, nullptr, main_menu_ui);
	Button* m3 = (Button*)CreateElement(BUTTON, iPoint(40, 160), SDL_Rect({ 641,166,228,68 }), nullptr, EXIT_GAME, nullptr, main_menu_ui);

	m1->Center(0, -70);
	m2->Center();
	m3->Center(0, 70);

	Label* title = (Label*)CreateElement(LABEL, iPoint(0, 50), { 0,0,0,0 }, "PAUSE MENU", NO_ACTION, nullptr, main_menu_ui);
	title->CenterX();

	Label* lm1 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "PLAY GAME", NO_ACTION, nullptr, m1);
	Label* lm2 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "SETTINGS", NO_ACTION, nullptr, m2);
	Label* lm3 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "EXIT GAME", NO_ACTION, nullptr, m3);

	lm1->Center();
	lm2->Center();
	lm3->Center();
	/////////////MAIN MENU FINISH //////////////////


	//////IN GAME UI //////////////
	in_game_ui = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,428,452 }), nullptr, NO_ACTION, (j1Module*)App, nullptr, true);
	Image* lives = (Image*)CreateElement(IMAGE, iPoint(10, 10), App->entitymanager->heart.idle.GetCurrentFrame(), nullptr, LIFE_SYSTEM, nullptr, in_game_ui);

	Image* green = (Image*)CreateElement(IMAGE, iPoint(700, 10), App->entitymanager->green_diamond.idle.GetCurrentFrame(), nullptr, INFO,nullptr, in_game_ui);
	Label* gl = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "X 50", INFO, nullptr, green);
	gl->Center(45, 5);
	
	Image* blue = (Image*)CreateElement(IMAGE, iPoint(800, 10), App->entitymanager->blue_diamond.idle.GetCurrentFrame(), nullptr, INFO, nullptr, in_game_ui);
	Label* bl = (Label*)CreateElement(LABEL, iPoint(845, 15), { 0,0,0,0 }, "X 100", INFO, nullptr, blue);
	bl->Center(45, 5);
	
	Label* score = (Label*)CreateElement(LABEL, iPoint(900, 15), { 0,0,0,0 }, "SCORE :", INFO, nullptr, in_game_ui);
	Label* ls = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "", SCORE, nullptr, score);
	ls->Center(45, 0);

	Label* timer = (Label*)CreateElement(LABEL, iPoint(900, 40), { 0,0,0,0 }, "TIME :", INFO, nullptr, in_game_ui);
	Label* tm = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "", GAME_TIMER, nullptr, timer);
	tm->Center(25, 0);

	Label* pl_name = (Label*)CreateElement(LABEL, iPoint(App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2, App->entitymanager->player_ref->position.y - 50), { 0,0,0,0 }, "--Kikime--", PLAYER_NAME, nullptr, in_game_ui);
	//////////IN GAME UI FINISH///////////////

	//in_game_pause = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w/2 - 214, App->render->viewport.h/2 - 226),SDL_Rect({ 28,542,428,452 }), nullptr, NO_ACTION, (j1Module*)App,nullptr,false);
	//Button* p1 = (Button*)CreateElement(BUTTON, iPoint(0, 20), SDL_Rect({ 641,166,228,68 }), nullptr, PAUSE, nullptr,  in_game_pause);
	//Button* p2 = (Button*)CreateElement(BUTTON, iPoint(40, 90), SDL_Rect({ 641,166,228,68 }), nullptr, SETTINGS,nullptr,  in_game_pause);
	//Button* p3 = (Button*)CreateElement(BUTTON, iPoint(40, 160), SDL_Rect({ 641,166,228,68 }), nullptr, EXIT_GAME, nullptr, in_game_pause);

	///////////////PAUSE MENU////////////////
	in_game_pause = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w/2 - 220, App->render->viewport.h/2 - 166),SDL_Rect({ 0,448,440,272 }), nullptr, NO_ACTION, nullptr,nullptr,false);
	
	//Not hardcoded
	game_over = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - game_over_image.rect[IDLE].w / 2, App->render->viewport.h / 2 - game_over_image.rect[IDLE].h / 2), game_over_image.rect[IDLE], nullptr, NO_ACTION, nullptr, nullptr, false);
	last_death = (Image*)CreateElement(IMAGE, iPoint(-20, -20), last_death_image.rect[IDLE], nullptr, LAST_DEATH, nullptr, nullptr, true);

	Button* b1 = (Button*)CreateButton({ 0,0 }, blue_button, PAUSE, nullptr, in_game_pause);
	Button* b2 = (Button*)CreateButton({ 0,0 }, green_button, SETTINGS, nullptr, in_game_pause);
	Button* b3 = (Button*)CreateButton({ 0,0 }, red_button, EXIT_GAME, nullptr, in_game_pause);
	Button* b4 = (Button*)CreateButton({ 0,0 }, red_button, MAIN_MENU, nullptr, in_game_pause);


	b1->Center(0,-70);
	b4->Center();
	b2->Center(0, 70);
	b3->Center(0, 140);

	Label* title_pause = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "PAUSE MENU", NO_ACTION, nullptr, in_game_pause);
	title_pause->CenterX();

	Label* l1 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "RESUME GAME", NO_ACTION, nullptr, b1);
	Label* l2 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "SETTINGS", NO_ACTION, nullptr, b2);
	Label* l3 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "EXIT GAME", NO_ACTION,nullptr,b3);
	Label* l4 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, "MAIN MENU", NO_ACTION, nullptr, b4);

	l1->Center();
	l2->Center();
	l3->Center();
	l4->Center();
	///////////////PAUSE MENU FINISH////////////////


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
	SDL_Texture* sprites = atlas;
	while (item != NULL)
	{
		item->data->PostUpdate();
		if (item->data->visible) {

			if (item->data->type == BUTTON) {
				Button* b = (Button*)item->data;
				switch (b->color)
				{
				case BLUE:
					sprites = blue;
					break;
				case RED:
					sprites = red;
					break;
				case YELLOW:
					sprites = yellow;
					break;
				case GREY:
					sprites = grey;
					break;
				case GREEN:
					sprites = green;
					break;
				default:
					sprites = atlas;
					break;
				}
			}

			item->data->Draw(sprites);
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
//Button* b3 = (Button*)CreateElement(BUTTON, iPoint(40, 160), SDL_Rect({ 641,166,228,68 }), nullptr, EXIT_GAME, nullptr, in_game_pause);
	
UIElement* j1Gui::CreateButton(iPoint pos,const Button &b, ActionType action, j1Module* callback, UIElement* parent)
{
	Button* elem = new Button(pos, b,action,parent);
	elements.add(elem);

	return elem;
}
UIElement* j1Gui::CreateElement(UIType type, iPoint pos, SDL_Rect rect, p2SString string, ActionType action, j1Module* callback, UIElement* parent, bool visible)
{
	UIElement* elem = nullptr;

	switch (type)
	{
  case IMAGE:
      
		elem = new Image(action,pos, rect, type, parent,visible);
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

	bool is_inside = (mouse.x >= element->position.x && mouse.x <= element->position.x + element->rect[element->state].w &&
					  mouse.y >= element->position.y && mouse.y <= element->position.y + element->rect[element->state].h);

	bool is_changing = false;
	UIState prev_state = element->state;
	
	if (element->state != CLICK_DOWN &&	is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) != KEY_DOWN)
	{
		element->state = HOVER;
		moving_element = false;
		//LOG("hover");
	}
	else if((element->state == HOVER || element->state == CLICK_DOWN) && is_inside && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN))
	{
		element->state = CLICK_DOWN;
		moving_element = true;
		//LOG("click");
	}
	else if (is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		element->state = CLICK_UP;
		moving_element = false;
		//LOG("click up");
	}
	else
	{
		element->state = IDLE;
		moving_element = false;
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

	if (moving_element) {
		iPoint final_motion;
		App->input->GetMouseMotion(final_motion.x, final_motion.y);
		last_motion -= final_motion;

		if (!last_motion.IsZero()) {
			element->position += final_motion;
		}
		last_motion = { final_motion.x, final_motion.y };
	}

}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}


