#include "ui/ui.hpp"

const mbl::platform::Input* mbl::ui::input_ptr;

float    mbl::ui::scale = 2;
float    mbl::ui::offset_x;
float    mbl::ui::offset_y;

mbl::render::Mesh	mbl::ui::rect_mesh;
mbl::render::Shader	mbl::ui::rect_shader;
mbl::render::Shader	mbl::ui::text_shader;
mbl::render::Font	mbl::ui::font;

mbl::render::Texture	mbl::ui::button_texture;
mbl::render::Texture	mbl::ui::button_highlighted_texture;

mbl::render::Texture	mbl::ui::text_field_texture;
mbl::render::Texture	mbl::ui::text_field_highlighted_texture;

mbl::render::Texture	mbl::ui::progress_texture;
mbl::render::Texture	mbl::ui::progress_background_texture;

std::vector<mbl::ui::DrawInfo>		mbl::ui::draws;
std::vector<mbl::ui::TextDrawInfo>	mbl::ui::text_draws;

std::string	mbl::ui::focused_text_input;
std::string	mbl::ui::dragging_slider;

void	mbl::ui::destroy()
{
}

void    mbl::ui::init(const std::string& font_path)
{
	font.load(font_path, 16);
	font.upload();

    vec2f rect_verts[] =
	{
		{0.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f}
	};

    mbl::loader::texture::stb::load("assets/textures/ui/progress.png", ui::progress_background_texture);
    ui::progress_background_texture.upload();
    mbl::loader::texture::stb::load("assets/textures/ui/progress_background.png", ui::progress_texture);
    ui::progress_texture.upload();

    mbl::loader::texture::stb::load("assets/textures/ui/button_highlighted.png", ui::button_highlighted_texture);
    ui::button_highlighted_texture.upload();
    mbl::loader::texture::stb::load("assets/textures/ui/button.png", ui::button_texture);
    ui::button_texture.upload();

    mbl::loader::texture::stb::load("assets/textures/ui/text_field_highlighted.png", ui::text_field_highlighted_texture);
    ui::text_field_highlighted_texture.upload();
    mbl::loader::texture::stb::load("assets/textures/ui/text_field.png", ui::text_field_texture);
    ui::text_field_texture.upload();

	mbl::ui::rect_mesh.set_sizeof_layout(sizeof(vec2f));
	mbl::ui::rect_mesh.add_vertex_layout(0, 2, GL_FLOAT, 0);
	mbl::ui::rect_mesh.add_vertex_data(reinterpret_cast<u8*>(rect_verts), sizeof(rect_verts));
	mbl::ui::rect_mesh.upload();
	mbl::ui::rect_shader.load("assets/shaders/ui/rect.vert", "assets/shaders/ui/rect.frag");
	mbl::ui::text_shader.load("assets/shaders/ui/text.vert", "assets/shaders/ui/text.frag");
}

void    mbl::ui::beginFrame(const mbl::platform::Input& input)
{
    mbl::ui::input_ptr = &input;
}

void    mbl::ui::render()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (mbl::ui::DrawInfo &d : mbl::ui::draws)
    {
        const vec2i &pos = d.pos;
        const vec2i &size = d.size;

        rect_shader.bind();
        mat4f model = mat4f::translate(vec3f(pos.x(), pos.y(), 0.0f)) * mat4f::scale(vec3f(size.x(), size.y(), 1.0f));
        rect_shader.setMat4("uModel", model);
        rect_shader.setMat4("uProj", mat4f::ortho(0.0f, mbl::ui::input_ptr->width(), mbl::ui::input_ptr->height(), 0.0f, -1.0f, 1.0f));
        rect_shader.setInt("uTex", 0);

        rect_shader.setVec2("uRectSize", size / ui::scale);
        rect_shader.setVec2("uTexSize", vec2f(200, 20));
        rect_shader.setVec2("uBorder", vec2f(3));

		if (d.texture)
			d.texture->bind(0);
		else
		    rect_shader.setVec3("uColor", vec3f(0, 1, 0));

        mbl::ui::rect_mesh.draw(GL_TRIANGLES);
    }
    for (mbl::ui::TextDrawInfo& d : mbl::ui::text_draws)
    {
	    const vec2i &pos = d.pos;
	    const vec2i &size = d.size;

	    text_shader.bind();
	    mat4f model = mat4f::translate(vec3f(pos.x(), pos.y(), 0.0f)) * mat4f::scale(vec3f(size.x(), size.y(), 1.0f));
	    text_shader.setMat4("uModel", model);
	    text_shader.setMat4("uProj", mat4f::ortho(0.0f, mbl::ui::input_ptr->width(), mbl::ui::input_ptr->height(), 0.0f, -1.0f, 1.0f));
	    text_shader.setInt("uTex", 0);
	    text_shader.setVec3("uColor", d.color);
	    text_shader.setInt("uBackground", d.background);
	    text_shader.setVec3("uBackgroundColor", d.background_color);
        mbl::ui::font.get_atlas().bind(0);
        text_shader.setVec4("uUV", d.uv);

	    mbl::ui::rect_mesh.draw(GL_TRIANGLES);
    }

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    draws.clear();
    text_draws.clear();
}

u32	mbl::ui::getFontSizeY()
{
	return (mbl::ui::font.get_char_size());
}

u32	mbl::ui::getFontSizeX(char c)
{
	return (mbl::ui::font.get_width(c));
}

u32	mbl::ui::getFontSizeX(const std::string &s)
{
	return (mbl::ui::font.get_width(s));
}

bool	mbl::ui::isOnBox(vec2i spos, vec2i ssize)
{
	mbl::utils::aabb2i  box = {.pos = spos, .size = ssize};

	return (mbl::utils::aabb2i::contains(box, vec2i(mbl::ui::input_ptr->mouseX(), mbl::ui::input_ptr->mouseY())));
}
