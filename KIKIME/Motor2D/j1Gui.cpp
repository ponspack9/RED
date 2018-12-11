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
	last_element_motion = { 0,0 };
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	BROFILER_CATEGORY("GUI->Awake", Profiler::Color::DarkGoldenRod)

	LOG("Loading GUI");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");
	blue_file_name = conf.child("blue").attribute("file").as_string("");
	red_file_name = conf.child("red").attribute("file").as_string();
	green_file_name = conf.child("green").attribute("file").as_string();
	yellow_file_name = conf.child("yellow").attribute("file").as_string();
	grey_file_name = conf.child("grey").attribute("file").as_string();

	moving_speed = conf.attribute("moving_elements_speed").as_int();

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

	//Creating slider pointer image (Green)
	n = conf.child("green").child("slider_pointer");
	for (SDL_Rect &r : slider_pointer_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("slider_pointer"))
			n = n.next_sibling("slider_pointer");
	}
	slider_pointer_button.type = UIType::BUTTON;
	slider_pointer_button.action = NO_ACTION;
	slider_pointer_button.parent = nullptr;
	slider_pointer_button.callback = nullptr;
	slider_pointer_button.visible = true;
	slider_pointer_button.color = GREEN;

	//Creating slider image
	n = conf.child("green").child("slider");
	for (SDL_Rect &r : slider_button.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("slider"))
			n = n.next_sibling("slider");
	}
	slider_button.type = UIType::BUTTON;
	slider_button.action = NO_ACTION;
	slider_button.parent = nullptr;
	slider_button.callback = nullptr;
	slider_button.visible = true;
	slider_button.color = GREEN;

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

	//pause window menu
	n = conf.child("atlas").child("window");
	for (SDL_Rect &r : white_window.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		if (n.next_sibling("window"))
			n = n.next_sibling("window");
	}

	// Blue diamond
	n = conf.child("atlas").child("blue_diamond");
	for (SDL_Rect &r : blue_diamond.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		blue_diamond.idle.PushBack(r);
		if (n.next_sibling("blue_diamond"))
			n = n.next_sibling("blue_diamond");

	}

	// Green diamond
	n = conf.child("atlas").child("green_diamond");
	for (SDL_Rect &r : blue_diamond.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		green_diamond.idle.PushBack(r);
		if (n.next_sibling("green_diamond"))
			n = n.next_sibling("green_diamond");
	}

	// Heart
	n = conf.child("atlas").child("heart");
	for (SDL_Rect &r : heart.rect)
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		heart.idle.PushBack(r);
		if (n.next_sibling("heart"))
			n = n.next_sibling("heart");
	}

	//READING ANIMATIONS
	//BLUE SHINE
	SDL_Rect r;
	n = conf.child("UIanimations").child("blue_shine");
	for (n; n; n = n.next_sibling("blue_shine"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		blue_diamond.blue_shine.PushBack(r);
	}
	blue_diamond.blue_shine.loop = true;

	//GREEN SHINE
	n = conf.child("UIanimations").child("green_shine");
	for (n; n; n = n.next_sibling("green_shine"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		green_diamond.green_shine.PushBack(r);
	}
	green_diamond.green_shine.loop = true;

	//HEART BLINK
	n = conf.child("UIanimations").child("heart_blink");
	for (n; n; n = n.next_sibling("heart_blink"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		heart.heart_blink.PushBack(r);
	}
	heart.heart_blink.loop = true;

	blue_diamond.blue_shine.speed = green_diamond.green_shine.speed = conf.child("UIanimations").attribute("animSpeed").as_float();
	heart.heart_blink.speed = blue_diamond.blue_shine.speed / 5;

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	BROFILER_CATEGORY("GUI->Start",Profiler::Color::Coral)

	atlas = App->tex->Load(atlas_file_name.GetString());
	blue = App->tex->Load(blue_file_name.GetString());
	red = App->tex->Load(red_file_name.GetString());
	green = App->tex->Load(green_file_name.GetString());
	yellow = App->tex->Load(yellow_file_name.GetString());
	grey = App->tex->Load(grey_file_name.GetString());

	SDL_RenderGetViewport(App->render->renderer, &App->render->viewport);
	
	/////////////MAIN MENU //////////////////

	main_menu_window = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }), nullptr, nullptr, NO_ACTION, (j1Module*)App, nullptr, true);
	
	main_menu_ui = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400), SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION, (j1Module*)App, main_menu_window, true);
	Button* start_button	= (Button*)CreateButton({ 0,0 }, blue_button, START, nullptr, main_menu_ui);
	Button* continue_button = (Button*)CreateButton({ 0,0 }, yellow_button, CONTINUE, nullptr, main_menu_ui);
	Button* settings_button = (Button*)CreateButton({ 0,0 }, green_button, SETTINGS, nullptr, main_menu_ui);
	Button* exit_button		= (Button*)CreateButton({ 0,0 }, red_button, EXIT_GAME, nullptr, main_menu_ui);
	
	start_button	 ->movable = true;
	continue_button	 ->movable = true;
	settings_button	 ->movable = true;
	exit_button    	 ->movable = true;

	start_button->Center(0, -140);
	continue_button->Center(0,-70);
	settings_button->Center();
	exit_button->Center(0, 70);

	Label* lstart_button	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "PLAY GAME", NO_ACTION, nullptr, start_button);
	Label* lcontinue_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "CONTINUE", NO_ACTION, nullptr, continue_button);
	Label* lsettings_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "SETTINGS", NO_ACTION, nullptr, settings_button);
	Label* lexit_button		= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "EXIT GAME", NO_ACTION, nullptr, exit_button);

	lstart_button->Center();
	lcontinue_button->Center();
	lsettings_button->Center();
	lexit_button->Center();

	//Credits window

	credits_window = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400), SDL_Rect({ 1000,1000,500,800 }), nullptr, nullptr, NO_ACTION, (j1Module*)App, main_menu_window, false);

	Button* credits_button = (Button*)CreateButton({ 50,50 }, red_button, CREDITS, nullptr, main_menu_window);

	Button* website_button = (Button*)CreateButton({ 50,50 }, green_button, WEBSITE, nullptr, credits_window);

	Label* lcredits_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "CREDITS", NO_ACTION, nullptr, credits_button);
	Label* lwebsite_button	= (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "Go to website", NO_ACTION, nullptr, website_button);
	Label* lcredits = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "Game developed by Oscar Pons and Oriol Sabate", NO_ACTION, nullptr, credits_window);

	website_button->Center(0, 70);
	lcredits_button->Center();
	lwebsite_button->Center();
	lcredits->Center();

	//Settings window
	settings_window = (Image*)CreateElement(IMAGE, { App->render->viewport.w / 2 - 250, App->render->viewport.h / 2 - 400 }, SDL_Rect({ 1000,1000,500,800 }), nullptr,nullptr, NO_ACTION, (j1Module*)App, main_menu_window, false);
	Button* settings_to_main = (Button*)CreateButton({ 0,0 }, red_button, SETTINGS, nullptr, settings_window);
	Label* lsettings_to_main = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "RETURN", NO_ACTION, nullptr, settings_to_main);
	settings_to_main->Center(0, 70);
	lsettings_to_main->Center();
	
	//Music Volume
	Button* volume_button = (Button*)CreateButton({ 0,-150 }, green_button, NO_ACTION, nullptr, settings_to_main);
	Label* lvolume_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "MUSIC VOLUME", NO_ACTION, nullptr, volume_button);
	volume_button->CenterX();
	lvolume_button->Center();

	Button* volume_slider = (Button*)CreateButton({ 0,volume_button->rect[IDLE].h+20 }, slider_button, NO_ACTION, nullptr, volume_button);
	Button* volume_slider_button = (Button*)CreateButton({ SDL_MIX_MAXVOLUME,0 }, slider_pointer_button, CHANGE_VOLUME, nullptr, volume_slider);
	volume_slider->CenterX();
	volume_slider_button->movable = true;

	//FX Volume
	Button* fx_volume_button = (Button*)CreateButton({ 0,-300 }, green_button, NO_ACTION, nullptr, settings_to_main);
	Label* fx_lvolume_button = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 }, nullptr, "FX VOLUME", NO_ACTION, nullptr, fx_volume_button);
	fx_volume_button->CenterX();
	fx_lvolume_button->Center();

	Button* fx_slider = (Button*)CreateButton({ 0,fx_volume_button->rect[IDLE].h + 20 }, slider_button, NO_ACTION, nullptr, fx_volume_button);
	Button* fx_slider_button = (Button*)CreateButton({ SDL_MIX_MAXVOLUME,0 }, slider_pointer_button, CHANGE_VOLUME_FX, nullptr, fx_slider);
	fx_slider->CenterX();
	fx_slider_button->movable = true;
	//slider_button->CenterX();


	//Image* greend = (Image*)CreateElement(IMAGE, iPoint(0, 0), green_diamond.idle.GetCurrentFrame(), &green_diamond, nullptr, INFO, nullptr, settings_window);
	//Label* gl = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "X 50", INFO, nullptr, greend);
	//
	//greend->Center(-60, -20);
	//gl->Center(45, 5);
	//
	//Image* blued = (Image*)CreateElement(IMAGE, iPoint(0, 0), blue_diamond.idle.GetCurrentFrame(), &blue_diamond, nullptr, INFO, nullptr, settings_window);
	//Label* bl = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "X 100", INFO, nullptr, blued);
	//
	//blued->Center(20, -20);
	//bl->Center(45, 5);

	/////////////MAIN MENU FINISH //////////////////

	/*
	To input text check the keyobard state then make a bool matrix or something similar then update if changed the specific key true
	*/

	//////IN GAME UI //////////////

	in_game_window = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,App->render->viewport.w,App->render->viewport.h }),nullptr, nullptr, NO_ACTION, (j1Module*)App, nullptr, true);
	in_game_ui = (Image*)CreateElement(IMAGE, iPoint(0, 0), SDL_Rect({ 1000,1000,428,452 }),nullptr, nullptr, NO_ACTION, (j1Module*)App, in_game_window, true);

	heart_ref = (Image*)CreateElement(IMAGE, iPoint(10, 10), heart.rect[IDLE], &heart, nullptr, LIFE_SYSTEM, nullptr, in_game_ui);

	green_ref = (Image*)CreateElement(IMAGE, iPoint(0, 15), green_diamond.idle.GetCurrentFrame(), &green_diamond, nullptr, DYNAMIC_INFO, nullptr, in_game_ui);
	Label* igl = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "X", INFO, nullptr, green_ref);
	Label* igl_count = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "", GREEN_COUNTER, nullptr, igl);
	green_ref->CenterX(700);
	igl->Center(30, 5);
	igl_count->Center(20, 0);
	
	blue_ref = (Image*)CreateElement(IMAGE, iPoint(0,15), blue_diamond.idle.GetCurrentFrame(), &blue_diamond, nullptr, DYNAMIC_INFO, nullptr, in_game_ui);
	Label* ibl = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "X", INFO, nullptr, blue_ref);
	Label* ibl_count = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "", BLUE_COUNTER, nullptr, ibl);
	blue_ref->CenterX(800);
	ibl->Center(30, 5);
	ibl_count->Center(20, 0);
	
	Label* score = (Label*)CreateElement(LABEL, iPoint(0, 20), { 0,0,0,0 }, nullptr, "SCORE :", INFO, nullptr, in_game_ui);
	Label* ls = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "", SCORE, nullptr, score);
	score->CenterX(950);
	ls->Center(45, 0);

	Label* timersec = (Label*)CreateElement(LABEL, iPoint(0, 15), { 0,0,0,0 },nullptr, ":", INFO, nullptr, in_game_ui);
	Label* tmi = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "", GAME_TIMER_MINS, nullptr, timersec);
	Label* tse = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "", GAME_TIMER_SECS, nullptr, timersec);
	timersec->CenterX(400);
	tmi->Center(-30, 0);
	tse->Center(25, 0);

	Label* pl_name = (Label*)CreateElement(LABEL, iPoint(App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2, App->entitymanager->player_ref->position.y - 50), { 0,0,0,0 },nullptr, "--Kikime--", PLAYER_NAME, nullptr, in_game_ui);
	//////////IN GAME UI FINISH///////////////

  /// Death related images
	game_over = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - game_over_image.rect[IDLE].w / 2, App->render->viewport.h / 2 - game_over_image.rect[IDLE].h / 2), game_over_image.rect[IDLE], &game_over_image, nullptr, NO_ACTION, nullptr, nullptr, false);
	last_death = (Image*)CreateElement(IMAGE, iPoint(-20, -20), last_death_image.rect[IDLE],&last_death_image, nullptr, LAST_DEATH, nullptr, nullptr, true);


	///////////////PAUSE MENU////////////////
	in_game_pause = (Image*)CreateElement(IMAGE, iPoint(App->render->viewport.w / 2 - 120, App->render->viewport.h / 2 - 166), white_window.rect[IDLE], &white_window, nullptr, NO_ACTION, nullptr, nullptr, false);	

	Button* b1 = (Button*)CreateButton({ 0,0 }, blue_button, PAUSE, nullptr, in_game_pause);
	Button* b2 = (Button*)CreateButton({ 0,0 }, green_button, SETTINGS, nullptr, in_game_pause);
	Button* b3 = (Button*)CreateButton({ 0,0 }, red_button, EXIT_GAME, nullptr, in_game_pause);
	Button* b4 = (Button*)CreateButton({ 0,0 }, red_button, MAIN_MENU, nullptr, in_game_pause);
	
	b1->Center(0,-100);
	b4->Center(0, -30);
	b2->Center(0, 40);
	b3->Center(0, 110);

	Label* title_pause = (Label*)CreateElement(LABEL, iPoint(0, 15), { 0,0,0,0 },nullptr, "PAUSE MENU", NO_ACTION, nullptr, in_game_pause);
	title_pause->CenterX();

	Label* l1 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "RESUME GAME", NO_ACTION, nullptr, b1);
	Label* l2 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "SETTINGS", NO_ACTION, nullptr, b2);
	Label* l3 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "EXIT GAME", NO_ACTION,nullptr,b3);
	Label* l4 = (Label*)CreateElement(LABEL, iPoint(0, 0), { 0,0,0,0 },nullptr, "MAIN MENU", NO_ACTION, nullptr, b4);

	l1->Center();
	l2->Center();
	l3->Center();
	l4->Center();
	///////////////PAUSE MENU FINISH////////////////

	/*windows.add(in_game_window);
	windows.add(main_menu_window);
	windows.add(credits_window);*/

	in_game_pause->SetInvisible();
	in_game_window->SetInvisible();
	credits_window->SetInvisible();
	settings_window->SetInvisible();
	in_game_ui->SetInvisible();
 
	return true;
}


// Update all guis
bool j1Gui::PreUpdate()
{
  BROFILER_CATEGORY("GUI->PreUpdate", Profiler::Color::DarkSalmon);
    
	bool has_changed = false;
	p2List_item<UIElement*>* item = elements.end;	

	while (item != nullptr)
	{
		if (item->data->visible) {

			item->data->PreUpdate();
			has_changed = HandleInput(item->data);
			if (has_changed) break;
		}

		item = item->prev;
	}

	return true;
}


// Called after all Updates
bool j1Gui::PostUpdate()
{
	BROFILER_CATEGORY("GUI->PostUpdate", Profiler::Color::Khaki)

	p2List_item<UIElement*>* item = elements.start;
	SDL_Texture* sprites = atlas;
	while (item != NULL)
	{
		if (item->data->visible) {
			if (item->data->type == BUTTON)
				int a = 1;
			item->data->PostUpdate();

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
			if (App->debug->show_colliders) item->data->DrawRect();
		}
		item = item->next;
	}
	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	BROFILER_CATEGORY("GUI->CleanUp", Profiler::Color::Magenta)

	LOG("Freeing GUI");

	p2List_item<UIElement*>* item = elements.start;

	while (item != nullptr)
	{
		delete item->data;

		item = item->next;
	}
	
	return true;
}

UIElement* j1Gui::CreateButton(iPoint pos,const Button &b, ActionType action, j1Module* callback, UIElement* parent)
{
	Button* elem = new Button(pos, b,action,parent);
	if (parent != nullptr) parent->sons.add(elem);
	elements.add(elem);

	return elem;
}

UIElement* j1Gui::CreateElement(UIType type, iPoint pos, SDL_Rect rect, Image* img, p2SString string, ActionType action, j1Module* callback, UIElement* parent, bool visible)
{
	UIElement* elem = nullptr;

	switch (type)
	{
  case IMAGE:
      
		elem = new Image(action,pos, rect, img, type, parent,visible);
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

	if (parent != nullptr) parent->sons.add(elem);
	elements.add(elem);

	return elem;
}

bool j1Gui::HandleInput(UIElement* element)
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
		//LOG("hover");
	}
	else if((element->state == HOVER || element->state == CLICK_DOWN) && is_inside && (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN))
	{
		element->state = CLICK_DOWN;
		App->input->GetMousePosition(mouseClick.x, mouseClick.y);
		//App->input->GetMousePosition(startDraging.x, startDraging.y);
		startDraging = element->initial_pos;
		element->is_moving = true;
		//LOG("click");
	}
	else if (is_inside && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
	{
		element->state = CLICK_UP;
		element->is_moving = false;
		//LOG("click up");
	}
	else
	{
		element->state = IDLE;
		//LOG("idle");
	}

    is_changing = (prev_state != element->state);
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
		element->is_moving = false;
	}
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

	if (element->is_moving && element->movable) 
	{
		if (element->action == CHANGE_VOLUME || element->action == CHANGE_VOLUME_FX) {
			element->state = HOVER;
			element->initial_pos.x = mouse.x - mouseClick.x + startDraging.x;

			if (element->initial_pos.x <= - 1) element->initial_pos.x = 0;
			else if (element->initial_pos.x >= SDL_MIX_MAXVOLUME+ 1) element->initial_pos.x = SDL_MIX_MAXVOLUME;

			if (element->action == CHANGE_VOLUME) App->ChangeMusicVolume(element->initial_pos.x);
			else App->ChangeFXVolume(element->initial_pos.x );
		}
		else element->initial_pos = mouse - mouseClick + startDraging;
	}

	return is_changing && element->type != LABEL;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}