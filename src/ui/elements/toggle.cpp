#include "ui/ui.hpp"

bool	mbl::ui::toggle(const std::string& label, bool& state, vec2i pos, vec2i size, Anchor anchor)
{
    vec2i spos;
	vec2i ssize;
    scalePosAndSize(spos, ssize, pos, size, anchor);

    bool	hovered = isOnBox(spos, ssize);

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .hovered = state});

    centeredScaledText(label, pos, size, anchor);

    if (mbl::ui::input_ptr->wasPressed(SDL_BUTTON_LEFT) && hovered)
    {
    	state = !state;
     	return (true);
    }

    return (false);
}
