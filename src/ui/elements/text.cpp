#include "ui/ui.hpp"

void	mbl::ui::text(const std::string& label, vec2f pos, Anchor anchor, vec3f color, bool background, vec3f background_color)
{
	int		total_width = mbl::ui::font.get_width(label);
	int		height = mbl::ui::font.get_char_size();

	float	base_x = pos.x() * mbl::ui::scale + (ui::input_ptr->width() * anchor.x()) - (total_width * ui::scale * anchor.x());
	float	base_y = pos.y() * mbl::ui::scale + (ui::input_ptr->height() * anchor.y()) - (height * ui::scale * anchor.y());
	float	cursor_x = base_x;

	for (char c : label)
	{
		int		width = mbl::ui::font.get_width(c);

		std::string key(1, c);

		if (mbl::ui::font.get_atlas().has(key))
		{
			vec2f spos = vec2f(cursor_x, base_y);
			vec2f ssize = vec2f(width * mbl::ui::scale, height * mbl::ui::scale);

			mbl::ui::text_draws.push_back({.pos = spos, .size = ssize, .uv = mbl::ui::font.get_atlas().uv(key), .background = background, .color = color, .background_color = background_color});
		}

		cursor_x += width * mbl::ui::scale;
	}
}
