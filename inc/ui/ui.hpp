#pragma once

#include "math/math.hpp"
#include "platform/Input.hpp"
#include "render/Font.hpp"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "render/Mesh.hpp"
#include "utils/AABB.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace mbl {

/// Static immediate-mode UI system: call beginFrame() once per frame, then the widget
/// functions (text/button/toggle/input/slider/progress_bar) to queue draws, then render().
/// Positions/sizes are given in a virtual target resolution (setTargetSize()) and scaled
/// to fit the actual window.


class ui
{
    public:
		using Anchor = vec2f;
		#define	ANCHOR_TOP_LEFT vec2f(0, 0)
		#define	ANCHOR_BOTTOM_LEFT vec2f(0, 1)
		#define	ANCHOR_TOP_RIGHT vec2f(1, 0)
		#define	ANCHOR_BOTTOM_RIGHT vec2f(1, 1)
		#define	ANCHOR_CENTER vec2f(0.5, 0.5)

        /// Loads the bitmap font used to render all text.
        static void    init(const std::string& font_path);
        static void		destroy();

        /// Starts a new frame; must be called before any widget function.
        static void    beginFrame(const platform::Input& input);
        /// Draws everything queued by widget calls since beginFrame().
        static void    render();

        /// Current pixel scale factor from virtual to real window size.
        static float	getScale();
        static u32		getFontSizeY();
        static u32		getFontSizeX(char c);
        static u32		getFontSizeX(const std::string &s);

        static void	text(const std::string& label, vec2f pos, Anchor anchor = vec2f(0.5), vec3f color = vec3f(1), bool background = false, vec3f background_color = vec3f(1));
        /// advance in [0, 1].
        static void	progress_bar(float advance, vec2f pos, vec2f size, Anchor anchor = vec2f(0.5));
        /// Returns true on the frame the button is clicked.
        static bool	button(const std::string& label, vec2f pos, vec2f size, Anchor anchor = vec2f(0.5));
        /// Toggles state on click; returns the resulting state.
        static bool	toggle(const std::string& label, bool& state, vec2f pos, vec2f size, Anchor anchor = vec2f(0.5));
        /// Editable text field; returns true while it has focus.
        static bool	input(const std::string& label, std::string& input, vec2f pos, vec2f size, Anchor anchor = vec2f(0.5));
        /// Draggable slider over [min, max]; returns true while being dragged.
        static bool	slider(const std::string& label, int& input, int min, int max, vec2f pos, vec2f size, Anchor anchor = vec2f(0.5));

    private:
        static vec2i	anchorOrigin(vec2i ssize, ui::Anchor anchor);
        static void		scalePosAndSize(vec2i& spos, vec2i& ssize, vec2i pos, vec2i size, ui::Anchor anchor);
        static void		centeredScaledTextY(const std::string& label, vec2i pos, vec2i size, ui::Anchor anchor);
        static void		centeredScaledText(const std::string& label, vec2i pos, vec2i size, ui::Anchor anchor);

        static bool	isOnBox(vec2i spos, vec2i ssize);
    private:
        struct  DrawInfo
        {
            vec2i   pos;
            vec2i   size;
            bool	hovered;
            bool	textured = false;
            vec4f	uv = vec4f(0, 0, 1, 1);
            bool	text = false;
            bool	text_background = false;
            vec3f	text_color = vec3f(1);
            vec3f	text_background_color = vec3f(1);
        };
        struct	TextDrawInfo
        {
	        vec2i   pos;
			vec2i   size;
			vec4f	uv = vec4f(0, 0, 1, 1);

			bool	background = false;
            vec3f	color = vec3f(1);
            vec3f	background_color = vec3f(1);
        };
    private:
        static const platform::Input* input_ptr;

        static float    scale;
        static float    offset_x;
        static float    offset_y;

        static render::Mesh    rect_mesh;
        static render::Shader  rect_shader;
        static render::Shader	text_shader;

        static render::Font	font;

        static std::vector<DrawInfo>  draws;
        static std::vector<TextDrawInfo>  text_draws;
        static std::string	focused_text_input;
        static std::string	dragging_slider;
};

}
