#include "ui/ui.hpp"

bool    mbl::ui::input(const std::string& label, std::string& input, vec2f pos, vec2f size, Anchor anchor)
{
	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));
    // scalePosAndSize(spos, ssize, pos, size, anchor);

    bool	hovered = isOnBox(spos, ssize);
    bool	focused = mbl::ui::focused_text_input == label;

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .hovered = focused});

    std::string	render_input = input;
    if (focused)
    	render_input += "_";
    // centeredScaledTextY(render_input, pos, size, anchor);

    if (mbl::ui::input_ptr->wasPressed(SDL_BUTTON_LEFT))
    {
    	if (focused)
     		mbl::ui::focused_text_input = "";
    	else if (hovered)
    		mbl::ui::focused_text_input = label;
    }

    const std::string	&text_input = mbl::ui::input_ptr->textInput();
    if (!text_input.empty() && focused)
    	input += text_input;

    if (mbl::ui::input_ptr->wasPressed(SDLK_BACKSPACE) && focused && !input.empty())
    	input = input.substr(0, input.size() - 1);

    return (false);
}
