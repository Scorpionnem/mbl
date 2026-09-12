#include "ui/ui.hpp"

bool    mbl::ui::button(const std::string& label, vec2i pos, vec2i size, Anchor anchor)
{
    vec2i spos;
    vec2i ssize;
    scalePosAndSize(spos, ssize, pos, size, anchor);

    bool	hovered = isOnBox(spos, ssize);

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .hovered = hovered});

    centeredScaledText(label, pos, size, anchor);

    if (mbl::ui::input_ptr->wasPressed(SDL_BUTTON_LEFT) && hovered)
        return (true);
    return (false);
}
