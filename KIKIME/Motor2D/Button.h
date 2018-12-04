#include "UIElement.h"

class Button : public UIElement
{
public:

	Button() {}
	Button(ActionType action, iPoint pos, SDL_Rect rect, UIType type, j1Module* callback);

	~Button() {}

	bool PreUpdate();
	bool PostUpdate();

	void HandleAction();
};