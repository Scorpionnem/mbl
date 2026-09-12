#pragma once

#include "render/Texture.hpp"

#include <vector>

namespace mbl::loader::texture::stb
{
    void    load(const std::string& path, std::vector<u8>& pixels, int& width, int& height, int& channels, GLenum& format);
	void	load(const std::string& path, mbl::render::Texture& texture);
}
