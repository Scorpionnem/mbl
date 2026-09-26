#pragma once

#include "loader/texture/stb.hpp"
#include "render/TextureAtlas.hpp"

#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <string>

namespace mbl { namespace render {

/// Bitmap font: slices a fixed grid of monospaced glyph cells out of one square texture,
/// trims each glyph to its actual (non-transparent) width, and packs them into a TextureAtlas.
class	Font
{
	public:
		Font() {}
		~Font() {}

		Font(const Font&) = delete;
		Font& operator=(const Font&) = delete;

		/// Loads the font texture; font_atlas_format is the grid size (e.g. 16 for a 16x16 glyph grid).
		void	load(const std::string& path, u32 font_atlas_format);
		void	upload();

		const TextureAtlas&	get_atlas() const {return (atlas);}
		int	get_char_size() const {return (char_size);}
		int	get_width(char c) const {return (widths[(u8)c]);}
		/// Total rendered width of the string at the font's native scale.
		int	get_width(const std::string& s) const
		{
			int	res = 0;
			for (char c : s)
				res += get_width(c);
			return (res);
		}

	private:
		std::vector<u8>	_extractTexture(const std::vector<u8>& pixels, int stride_width, int channels, int off_x, int off_y);
		bool			_isEmptyColumn(const std::vector<u8>& cell, int channels, int x);
		int				_measureWidth(const std::vector<u8>& cell, int channels, int& out_left);
		std::vector<u8>	_cropTexture(const std::vector<u8>& cell, int channels, int left, int width);

		TextureAtlas	atlas;
		int				char_size = 0;
		int				widths[256] = {};
};

}}
