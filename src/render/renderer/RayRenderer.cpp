#include "render/renderer/RayRenderer.hpp"

mbl::render::Shader*	mbl::render::renderer::RayRenderer::_ext_shader;
mbl::render::Mesh*		mbl::render::renderer::RayRenderer::_ext_mesh;
mbl::render::Shader		mbl::render::renderer::RayRenderer::_int_shader;
mbl::render::Mesh		mbl::render::renderer::RayRenderer::_int_mesh;

void	mbl::render::renderer::RayRenderer::gen_render_data(const char* vert_path, const char* frag_path,
							render::Mesh* ext_mesh, render::Shader* ext_shader)
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
