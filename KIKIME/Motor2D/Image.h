#include "UIElement.h"

class Image :public UIElement
{
public:

	Image() {}
	Image(iPoint pos, SDL_Rect rect, UIType type, ActionType action);

	~Image() {}

	bool PreUpdate();
	bool PostUpdate();

	void Draw(SDL_Texture* sprites);
};
