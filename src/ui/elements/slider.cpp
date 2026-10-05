#include "ui/ui.hpp"

bool	mbl::ui::slider(const std::string& label, int& input, int min, int max, vec2f pos, vec2f size, Anchor anchor)
{
	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));

    float	frac = (max != min) ? static_cast<float>(input - min) / static_cast<float>(max - min) : 0.0f;
    frac = std::clamp(frac, 0.0f, 1.0f);

    float	handleWidth = 10.0f;

    vec2f	sliderssize = vec2f(handleWidth, size.y()) * mbl::ui::scale;
    vec2f	sliderspos = vec2f(pos.x() + static_cast<int>(std::round(frac * (size.x() - handleWidth))), pos.y()) * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));;

    bool	hovered = isOnBox(spos, ssize);
    bool	dragging = mbl::ui::dragging_slider == label;

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .texture = hovered || dragging ? &slider_highlighted_texture : &slider_texture});

    mbl::ui::draws.push_back({.pos = sliderspos, .size = sliderssize, .texture = hovered || dragging ? &button_highlighted_texture : &button_texture});

	if (input_ptr->wasPressed(SDL_BUTTON_LEFT) && !dragging && hovered && !clicked_frame && dragging_slider.empty())
	{
	    dragging_slider = label;
	    dragging = true;
	}
	else if (input_ptr->isDown(SDL_BUTTON_LEFT) && dragging)
	{
	    float offset = (input_ptr->mouseX() - spos.x()) / ssize.x();
	    offset = std::clamp(offset, 0.0f, 1.0f);
	    input = min + static_cast<int>(std::round((max - min) * offset));
	    clicked_frame = true;
	}
	else if (dragging)
		ui::dragging_slider.clear();

    return (false);
}
