#include "ui/ui.hpp"

void	mbl::ui::progress_bar(float advance, vec2f pos, vec2f size, Anchor anchor)
{
	advance = std::clamp(advance, 0.0f, 1.0f);

	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));
    // scalePosAndSize(spos, ssize, pos, size, anchor);

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .hovered = false});

    vec3f adv_ssize = vec2f(ssize.x() * advance, ssize.y());
    vec3f adv_spos = spos;

    mbl::ui::draws.push_back({.pos = adv_spos, .size = adv_ssize, .hovered = true});
}
