#include "UIElement.h"

class Button : public UIElement
{
public:

	Button() {}
	Button(iPoint pos,const Button &b,ActionType action, UIElement* parent = nullptr);
	Button(ActionType action, iPoint pos, SDL_Rect rect, UIType type, j1Module* callback, UIElement* parent, bool visible);

	~Button() {}

	void HandleAction();

	UIColor color;
};