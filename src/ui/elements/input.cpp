#include "ui/ui.hpp"

bool    mbl::ui::input(const std::string& label, std::string& input, vec2f pos, vec2f size, Anchor anchor)
{
	vec2f ssize = size * mbl::ui::scale;
    vec2f spos = pos * mbl::ui::scale + ((vec2f(ui::input_ptr->size()) * anchor) - (ssize * anchor));
    // scalePosAndSize(spos, ssize, pos, size, anchor);

    bool	hovered = isOnBox(spos, ssize);
    bool	focused = mbl::ui::focused_text_input == label;

    mbl::ui::draws.push_back({.pos = spos, .size = ssize, .texture = hovered || focused || hovered ? &text_field_highlighted_texture : &text_field_texture});

    std::string	render_input = input;
    if (focused)
    	render_input += "_";

    if (input.empty())
    {
   		vec2f	text_pos = spos + ssize / 2 - vec2f(mbl::ui::font.get_width(label), mbl::ui::font.get_char_size()) * ui::scale / 2;
    	mbl::ui::raw_text(label, text_pos, ui::scale, vec3f(0.6));
    }
    else
    {
    	float border_size = 3;
  		vec2f	text_pos = spos + vec2f(0, ssize.y() / 2.0) - vec2f(-border_size * ui::scale, mbl::ui::font.get_char_size() * ui::scale / 2);
   		mbl::ui::raw_text(input, text_pos, ui::scale);
    }

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
