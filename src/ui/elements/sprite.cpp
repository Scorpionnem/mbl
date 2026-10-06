#include "ui/ui.hpp"

void	mbl::ui::sprite(mbl::render::Texture* tex, vec2f pos, vec2f size, ui::Anchor anchor)
{
	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));

    mbl::ui::sprite_draws.push_back({.pos = spos, .size = ssize, .texture = tex});
}
