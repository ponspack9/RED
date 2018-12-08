#include "UIElement.h"

class Image :public UIElement
{
public:

	Image() {}
	Image(ActionType action, iPoint pos, SDL_Rect rect, UIType type, UIElement* parent, bool visible);

	~Image() {}

	void Draw(SDL_Texture* sprites);

};
