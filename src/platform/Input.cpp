#include "platform/Input.hpp"

mbl::platform::Input::Input() : _mouseDX(0.0f), _mouseDY(0.0f)
{
	_pressed.clear();
	_down.clear();
}

void    mbl::platform::Input::beginFrame()
{
	_resize = false;
	_close = false;
	_pressed.clear();
    _mouseDX = 0.0f;
    _mouseDY = 0.0f;
    _textInput.clear();
}

void    mbl::platform::Input::press(int action)
{
    _down[action] = true;
    _pressed[action] = true;
}

void    mbl::platform::Input::release(int action)
{
    _down[action] = false;
}

void    mbl::platform::Input::addMouseDelta(float dx, float dy)
{
    _mouseDX += dx;
    _mouseDY += dy;
}

bool    mbl::platform::Input::isDown(int action) const
{
    return (_down[action]);
}

bool    mbl::platform::Input::wasPressed(int action) const
{
    return (_pressed[action]);
}

float   mbl::platform::Input::mouseDX() const
{
    return (_mouseDX);
}

float   mbl::platform::Input::mouseDY() const
{
    return (_mouseDY);
}

int   mbl::platform::Input::mouseX() const
{
    return (_mouseX);
}

int   mbl::platform::Input::mouseY() const
{
    return (_mouseY);
}

bool    mbl::platform::Input::close() const
{
    return (_close);
}

bool	mbl::platform::Input::resize() const
{
	return (_resize);
}

double    mbl::platform::Input::delta() const
{
    return (_delta);
}

double    mbl::platform::Input::aspect() const
{
    return (_aspect);
}

u32    mbl::platform::Input::width() const
{
    return (_width);
}

vec2i	mbl::platform::Input::size() const
{
	return (vec2i(_width, _height));
}

u32    mbl::platform::Input::height() const
{
    return (_height);
}
