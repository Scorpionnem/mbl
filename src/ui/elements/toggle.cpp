#include "ui/ui.hpp"

bool	mbl::ui::toggle(const std::string& label, bool& state, vec2f pos, vec2f size, Anchor anchor)
{
	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));

    bool	hovered = isOnBox(spos, ssize);

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .texture = hovered || state ? &button_highlighted_texture : &button_texture});

   	vec2f	text_pos = spos + ssize / 2 - vec2f(mbl::ui::font.get_width(label), mbl::ui::font.get_char_size()) * ui::scale / 2;
	mbl::ui::raw_text(label, text_pos, ui::scale);

    if (mbl::ui::input_ptr->wasPressed(SDL_BUTTON_LEFT) && hovered)
    {
    	state = !state;
     	return (true);
    }

    return (false);
}
