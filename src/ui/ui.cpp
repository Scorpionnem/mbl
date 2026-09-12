#include "ui/ui.hpp"

const mbl::platform::Input* mbl::ui::input_ptr;

u32 mbl::ui::target_width;
u32 mbl::ui::target_height;
u32 mbl::ui::target_font_scale = 1;

float    mbl::ui::scale;
float    mbl::ui::offset_x;
float    mbl::ui::offset_y;

mbl::render::Mesh    mbl::ui::rect_mesh;
mbl::render::Shader  mbl::ui::rect_shader;
mbl::render::Shader  mbl::ui::text_shader;
mbl::render::Font	mbl::ui::font;

std::vector<mbl::ui::DrawInfo>		mbl::ui::draws;
std::vector<mbl::ui::TextDrawInfo>	mbl::ui::text_draws;

std::string	mbl::ui::focused_text_input;
std::string	mbl::ui::dragging_slider;

void	mbl::ui::setTargetFontScale(u32 scale)
{
	mbl::ui::target_font_scale = scale;
}

void    mbl::ui::setTargetSize(u32 width, u32 height)
{
    mbl::ui::target_width = width;
    mbl::ui::target_height = height;
}

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

	mbl::ui::rect_mesh.set_sizeof_layout(sizeof(vec2f));
	mbl::ui::rect_mesh.add_vertex_layout(0, 2, GL_FLOAT, 0);
	mbl::ui::rect_mesh.add_vertex_data(reinterpret_cast<u8*>(rect_verts), sizeof(rect_verts));
	mbl::ui::rect_mesh.upload();
	mbl::ui::rect_shader.load("assets/shaders/ui/rect.vert", "assets/shaders/ui/rect.frag");
	mbl::ui::text_shader.load("assets/shaders/ui/text.vert", "assets/shaders/ui/text.frag");
}

vec2i	mbl::ui::anchorOrigin(vec2i ssize, mbl::ui::Anchor anchor)
{
    switch (anchor)
    {
        case mbl::ui::Anchor::TOP_LEFT:
            return (vec2i(0, 0));
        case mbl::ui::Anchor::TOP_RIGHT:
        return (vec2i(mbl::ui::input_ptr->width() - ssize.x(), 0));
        case mbl::ui::Anchor::BOTTOM_LEFT:
        return (vec2i(0, mbl::ui::input_ptr->height() - ssize.y()));
        case mbl::ui::Anchor::BOTTOM_RIGHT:
        return (vec2i(mbl::ui::input_ptr->width() - ssize.x(), mbl::ui::input_ptr->height() - ssize.y()));
        default:
            return (vec2i(mbl::ui::offset_x, mbl::ui::offset_y));
    }
}

float	mbl::ui::getScale()
{
	float scaleX = static_cast<float>(mbl::ui::input_ptr->width()) / mbl::ui::target_width;
	float scaleY = static_cast<float>(mbl::ui::input_ptr->height()) / mbl::ui::target_height;

	return (std::max(1.0f, std::round(std::min(scaleX, scaleY))));
}

void    mbl::ui::beginFrame(const mbl::platform::Input& input)
{
    mbl::ui::input_ptr = &input;

    mbl::ui::scale = getScale();
    mbl::ui::offset_x = (mbl::ui::input_ptr->width()  - mbl::ui::target_width  * mbl::ui::scale) * 0.5f;
    mbl::ui::offset_y = (mbl::ui::input_ptr->height() - mbl::ui::target_height * mbl::ui::scale) * 0.5f;
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
        rect_shader.setInt("uUseTex", d.textured ? 1 : 0);
        rect_shader.setVec3("uColor", d.text_color);
        rect_shader.setInt("uBackground", d.text_background);
        rect_shader.setVec3("uBackgroundColor", d.text_background_color);

        if (d.textured)
        {
            mbl::ui::font.get_atlas().bind(0);
            rect_shader.setVec4("uUV", d.uv);
        }
        else
        {
            rect_shader.setVec3("uColor", d.hovered ? vec3f(0, 1, 0) : vec3f(1, 0, 0));
        }

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
	return (mbl::ui::font.get_char_size() * mbl::ui::target_font_scale);
}

u32	mbl::ui::getFontSizeX(char c)
{
	return (mbl::ui::font.get_width(c) * mbl::ui::target_font_scale);
}

u32	mbl::ui::getFontSizeX(const std::string &s)
{
	return (mbl::ui::font.get_width(s) * mbl::ui::target_font_scale);
}

void	mbl::ui::centeredScaledText(const std::string& label, vec2i pos, vec2i size, mbl::ui::Anchor anchor)
{
	vec2i	text_pos;
    text_pos.x() = pos.x() + size.x() / 2 - (mbl::ui::font.get_width(label) * (int)mbl::ui::target_font_scale) / 2;
    text_pos.y() = pos.y() + size.y() / 2 - (mbl::ui::font.get_char_size() * (int)mbl::ui::target_font_scale) / 2;
    mbl::ui::text(label, text_pos, anchor);
}

void	mbl::ui::centeredScaledTextY(const std::string& label, vec2i pos, vec2i size, mbl::ui::Anchor anchor)
{
	vec2i	text_pos;
    text_pos.x() = pos.x();
    text_pos.y() = pos.y() + size.y() / 2 - (mbl::ui::font.get_char_size() * (int)mbl::ui::target_font_scale) / 2;
    mbl::ui::text(label, text_pos, anchor);
}

void	mbl::ui::scalePosAndSize(vec2i& spos, vec2i& ssize, vec2i pos, vec2i size, mbl::ui::Anchor anchor)
{
	ssize = vec2i(size.x() * mbl::ui::scale, size.y() * mbl::ui::scale);
    vec2i origin = anchorOrigin(ssize, anchor);
    spos = vec2i(pos.x() * mbl::ui::scale + origin.x(), pos.y() * mbl::ui::scale + origin.y());
}

bool	mbl::ui::isOnBox(vec2i spos, vec2i ssize)
{
	mbl::utils::aabb2i  box = {.min = spos, .max = spos + ssize};

	return (mbl::utils::aabb2i::contains(box, vec2i(mbl::ui::input_ptr->mouseX(), mbl::ui::input_ptr->mouseY())));
}
