#pragma once

#include "render/Texture.hpp"

#include <vector>

/// Thin wrapper around stb_image for loading common image formats (PNG, JPG, ...).
namespace mbl::loader::texture::stb
{
    /// Decodes the image at `path` into raw pixel data.
    void    load(const std::string& path, std::vector<u8>& pixels, int& width, int& height, int& channels, GLenum& format);
	/// Decodes the image at `path` directly into a Texture (does not upload it).
	void	load(const std::string& path, mbl::render::Texture& texture);
}
