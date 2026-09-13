#pragma once

#include "math/math.hpp"
#include <unordered_map>

namespace mbl { namespace platform {

class Window;

/// Per-frame input state, polled and updated by Window::pollEvents().
class Input
{
	public:
		Input();

		/// True while the key/button (SDL scancode or button constant) is held down.
		bool	isDown(int action) const;
		/// True only on the frame the key/button was first pressed.
		bool	wasPressed(int action) const;
		float	mouseDX() const;
		float	mouseDY() const;
		int		mouseX() const;
		int		mouseY() const;
		/// True the frame the window was asked to close.
		bool	close() const;
		/// True the frame the window was resized.
		bool	resize() const;
        /// Frame time in seconds.
        double  delta() const;
        double  aspect() const;
        u32     width() const;
        u32     height() const;
        vec2i     size() const;

        const std::string   &textInput() const {return (_textInput);};

	private:
		void	beginFrame();
		void	press(int action);
		void	release(int action);
		void	addMouseDelta(float dx, float dy);

		mutable std::unordered_map<int, bool>	_down;
		mutable std::unordered_map<int, bool>	_pressed;
		float	_mouseDX;
		float	_mouseDY;
		int		_mouseX;
		int		_mouseY;

        double  _delta;
        double  _aspect;

        u32 _width;
        u32 _height;

        bool    _close;
		bool	_resize;

		std::string _textInput;

		friend Window;
};

}}
