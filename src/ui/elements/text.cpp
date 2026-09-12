#include "ui/ui.hpp"

void	mbl::ui::text(const std::string& label, vec2i pos, Anchor anchor, vec3f color, bool background, vec3f background_color)
{
	int	char_size = mbl::ui::font.get_char_size();
	float	char_scale = target_font_scale * mbl::ui::scale;

	int	total_width = mbl::ui::font.get_width(label);

	vec2i	origin = anchorOrigin(vec2i(total_width * char_scale, char_size * char_scale), anchor);
	float	base_x = pos.x() * mbl::ui::scale + origin.x();
	float	base_y = pos.y() * mbl::ui::scale + origin.y();
	float	cursor_x = base_x;

	for (char c : label)
	{
		int	width = mbl::ui::font.get_width(c);
		std::string key(1, c);

		if (mbl::ui::font.get_atlas().has(key))
		{
			vec2i spos = vec2i(cursor_x, base_y);
			vec2i ssize = vec2i(width * char_scale, char_size * char_scale);

			mbl::ui::text_draws.push_back({.pos = spos, .size = ssize, .uv = mbl::ui::font.get_atlas().uv(key), .background = background, .color = color, .background_color = background_color});
		}

		cursor_x += width * char_scale;
	}
}
