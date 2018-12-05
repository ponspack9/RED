#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "UIElement.h"
#include "Image.h"
#include "Button.h"
#include "Label.h"
#include "j1App.h"

#define CURSOR_WIDTH 2

// ---------------------------------------------------
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

	// TODO 2: Create the factory methods
	// Gui creation functions
	UIElement* CreateElement(UIType type, iPoint pos, p2SString string = nullptr, ActionType action = NO_ACTION, j1Module* callback = nullptr);

	void HandleInput(UIElement* element);

	const SDL_Texture* GetAtlas() const;

private:

	p2List<UIElement*>  elements;

	SDL_Texture*		atlas;
	p2SString			atlas_file_name;
};

#endif // __j1GUI_H__