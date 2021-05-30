#pragma once

#include "Gear/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Color : public Vec4
{
public:
	using Vec4::Vec4;

	static Color White;
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Yellow;
	static Color Cyan;
	static Color Magenta;
	static Color Silver;
	static Color LightSteelBlue;
	static Color DarkBlue;

	static Color random();
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
