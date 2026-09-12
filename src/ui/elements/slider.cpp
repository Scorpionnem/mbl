#include "ui/ui.hpp"

bool	mbl::ui::slider(const std::string& label, int& input, int min, int max, vec2i pos, vec2i size, Anchor anchor)
{
	vec2i spos;
	vec2i ssize;
    scalePosAndSize(spos, ssize, pos, size, anchor);

    float	frac = (max != min) ? static_cast<float>(input - min) / static_cast<float>(max - min) : 0.0f;
    frac = std::clamp(frac, 0.0f, 1.0f);

    int	handleWidth = std::max(1, size.x() / 10);
    vec2i	sliderpos = vec2i(pos.x() + static_cast<int>(std::round(frac * (size.x() - handleWidth))), pos.y());
    vec2i	slidersize = vec2i(handleWidth, size.y());

    vec2i	sliderspos;
    vec2i	sliderssize;
    scalePosAndSize(sliderspos, sliderssize, sliderpos, slidersize, anchor);

    bool	hovered = isOnBox(spos, ssize);
    bool	dragging = mbl::ui::dragging_slider == label;

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .hovered = false});
    mbl::ui::draws.push_back({.pos = sliderspos, .size = sliderssize, .hovered = hovered || dragging});

    centeredScaledText(std::to_string(input), pos, size, anchor);

    if (mbl::ui::input_ptr->isDown(SDL_BUTTON_LEFT))
    {
    	if (!dragging && hovered && mbl::ui::dragging_slider.empty())
    	{
    		mbl::ui::dragging_slider = label;
    		dragging = true;
    	}

    	if (dragging)
    	{
    		float	offset = (static_cast<float>(mbl::ui::input_ptr->mouseX()) - static_cast<float>(spos.x())) / static_cast<float>(ssize.x());
    		offset = std::clamp(offset, 0.0f, 1.0f);

    		input = min + static_cast<int>(std::round((max - min) * offset));
    	}
    }
    else if (dragging)
    	mbl::ui::dragging_slider = "";

    return (false);
}
