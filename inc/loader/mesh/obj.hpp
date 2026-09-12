#pragma once

#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"
#include <string>

/// Wavefront .obj loader.
namespace mbl::loader::mesh::obj
{
	struct	Vertex
	{
		vec3f	pos;
		vec3f	normal;
		vec2f	uv;
	};

	/// Parses the .obj at `path` into `mesh`, packing any referenced material textures into `atlas`.
	void	load(const std::string& path, mbl::render::Mesh& mesh, mbl::render::TextureAtlas &atlas);
}
