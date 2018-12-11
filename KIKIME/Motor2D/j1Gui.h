#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "UIElement.h"
#include "Image.h"
#include "Button.h"
#include "Label.h"
#include "j1App.h"
#include "j1EntityManager.h"


class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	UIElement * CreateButton(iPoint pos, const Button & b, ActionType action, j1Module * callback, UIElement * parent);

	// Gui creation functions
	UIElement* CreateElement(UIType type, iPoint pos, SDL_Rect rect, p2SString string = nullptr, ActionType action = NO_ACTION, j1Module* callback = nullptr, UIElement* parent = nullptr, bool visible = true);

	void HandleInput(UIElement* element);

	const SDL_Texture* GetAtlas() const;

	Image*			in_game_ui;
	Image*			main_menu_ui;
	Image*			in_game_pause;

	Image*			main_menu_window;
	Image*			in_game_window;
	Image*			credits_window;
	Image*			settings_window;

	Image*			game_over;
	Image*			last_death;
	Image			white_window;

private:

	p2List<UIElement*>  elements;

	SDL_Texture*		atlas;
	SDL_Texture*		blue;
	SDL_Texture*		red;
	SDL_Texture*		green;
	SDL_Texture*		yellow;
	SDL_Texture*		grey;

	p2SString			atlas_file_name;
	p2SString			blue_file_name;
	p2SString			red_file_name;
	p2SString			green_file_name;
	p2SString			yellow_file_name;
	p2SString			grey_file_name;


	Button				blue_button;
	Button				red_button;
	Button				green_button;
	Button				yellow_button;
	Button				grey_button;
	Button				slider_pointer_button;

	Image				game_over_image;
	Image				last_death_image;

	bool				moving_element;
	iPoint				last_element_motion;
	int					moving_speed;
	iPoint				camera_motion;

};

#endif // __j1GUI_H__