#include "UIElement.h"

class Image :public UIElement
{
public:

	Image() {}
	Image(iPoint pos, SDL_Rect rect, UIType type, UIElement* parent, bool visible);

	~Image() {}
};
