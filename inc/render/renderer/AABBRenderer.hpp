#pragma once

#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "utils/AABB.hpp"

namespace mbl::render::renderer
{
struct AABBRenderer
{
	/// Builds a wireframe (GL_LINES) unit-cube mesh for drawing a 3D box; N == 3 only.
    static void	gen_render_data(const char* vert_path = "assets/shaders/aabb.vert", const char* frag_path = "assets/shaders/aabb.frag",
    							render::Mesh* ext_mesh = nullptr, render::Shader* ext_shader = nullptr)
	{
		if (ext_mesh)
			_ext_mesh = ext_mesh;
		if (ext_shader)
			_ext_shader = ext_shader;

		render::Mesh*	mesh = _ext_mesh ? _ext_mesh : &_int_mesh;
		render::Shader*	shader = _ext_shader ? _ext_shader : &_int_shader;

		shader->load(vert_path, frag_path);

		mesh->set_sizeof_layout(sizeof(vec3f));
		mesh->add_vertex_layout(0, 3, GL_FLOAT, 0);

		vec3f vertices[] = {
			{0,0,0}, {1,0,0},
			{0,0,0}, {0,1,0},
			{0,0,0}, {0,0,1},
			{0,1,0}, {1,1,0},
			{0,1,0}, {0,1,1},
			{1,1,1}, {0,1,1},
			{1,1,1}, {1,0,1},
			{1,1,1}, {1,1,0},
			{0,1,1}, {0,0,1},
			{0,0,1}, {1,0,1},
			{1,0,1}, {1,0,0},
			{1,1,0}, {1,0,0},
		};

		mesh->add_vertex_data(reinterpret_cast<u8*>(vertices), sizeof(vertices));
		mesh->upload();
	}

	/// Draws `aabb` as a wireframe box
	template <std::size_t NN, typename TT>
	static void draw(utils::AABB<NN, TT> aabb, const render::Camera& cam, vec3f color = vec3f(1))
	{
		render::Mesh*	mesh = _ext_mesh ? _ext_mesh : &_int_mesh;
		render::Shader*	shader = _ext_shader ? _ext_shader : &_int_shader;

		shader->bind();
		shader->setMat4("uProj", cam.getProjectionMatrix());
		shader->setMat4("uView", cam.getViewMatrix());
		shader->setMat4("uModel", mat4f::translate(vec3f(aabb.pos)) * mat4f::scale(vec3f(aabb.size)));
		shader->setVec3("uCol", color);
		mesh->draw(GL_LINES);
	}

	private:
		static render::Shader*	_ext_shader;
		static render::Mesh*	_ext_mesh;

		static render::Shader	_int_shader;
		static render::Mesh		_int_mesh;
};
}
