#pragma once

#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include <string>

namespace mbl::loader::mesh::obj
{
	struct	Vertex
	{
		vec3f	pos;
		vec3f	normal;
		vec2f	uv;
	};

	void	load(const std::string& path, mbl::render::Mesh& mesh, mbl::render::TextureAtlas &atlas);
}
