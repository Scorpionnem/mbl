#pragma once

#include "render/Mesh.hpp"
#include "render/Shader.hpp"
#include "math/ray.hpp"
#include "render/Camera.hpp"

namespace mbl::render::renderer
{
struct RayRenderer
{
    static void	gen_render_data(const char* vert_path = "assets/shaders/ray.vert", const char* frag_path = "assets/shaders/ray.frag",
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
		};

		mesh->add_vertex_data(reinterpret_cast<u8*>(vertices), sizeof(vertices));
		mesh->upload();
	}

	template <std::size_t NN, typename TT>
	static void draw(const ray<NN, TT>& ray, const render::Camera& cam, vec3f color = vec3f(1))
	{
		render::Mesh*	mesh = _ext_mesh ? _ext_mesh : &_int_mesh;
		render::Shader*	shader = _ext_shader ? _ext_shader : &_int_shader;

		shader->bind();
		shader->setMat4("uProj", cam.getProjectionMatrix());
		shader->setMat4("uView", cam.getViewMatrix());
		shader->setVec3("uCol", color);
		shader->setVec3("uPosA", ray.orig);
		shader->setVec3("uPosB", ray.orig + ray.dir);
		mesh->draw(GL_LINES);
	}

	template <std::size_t NN, typename TT>
	static void draw(const vec<NN, TT>& start, const vec<NN, TT>& end, const render::Camera& cam, vec3f color = vec3f(1))
	{
		render::Mesh*	mesh = _ext_mesh ? _ext_mesh : &_int_mesh;
		render::Shader*	shader = _ext_shader ? _ext_shader : &_int_shader;

		shader->bind();
		shader->setMat4("uProj", cam.getProjectionMatrix());
		shader->setMat4("uView", cam.getViewMatrix());
		shader->setVec3("uCol", color);
		shader->setVec3("uPosA", start);
		shader->setVec3("uPosB", end);
		mesh->draw(GL_LINES);
	}

	private:
		static render::Shader*	_ext_shader;
		static render::Mesh*	_ext_mesh;

		static render::Shader	_int_shader;
		static render::Mesh		_int_mesh;
};
}
