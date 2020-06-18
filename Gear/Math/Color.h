#pragma once

#include "Vector4.h"

namespaceStart(Gear)
namespaceStart(Color)

static const Math::Vec4 White(1.0f, 1.0f, 1.0f, 1.0f);
static const Math::Vec4 Black(0.0f, 0.0f, 0.0f, 1.0f);
static const Math::Vec4 Red(1.0f, 0.0f, 0.0f, 1.0f);
static const Math::Vec4 Green(0.0f, 1.0f, 0.0f, 1.0f);
static const Math::Vec4 Blue(0.0f, 0.0f, 1.0f, 1.0f);
static const Math::Vec4 Yellow(1.0f, 1.0f, 0.0f, 1.0f);
static const Math::Vec4 Cyan(0.0f, 1.0f, 1.0f, 1.0f);
static const Math::Vec4 Magenta(1.0f, 0.0f, 1.0f, 1.0f);
static const Math::Vec4 Silver(0.75f, 0.75f, 0.75f, 1.0f);
static const Math::Vec4 LightSteelBlue(0.69f, 0.77f, 0.87f, 1.0f);
static const Math::Vec4 DarkBlue(0.0f, 0.125f, 0.3f, 1.0f);

inline Math::Vec4 randomColor()
{
	static std::default_random_engine s_Engine((uint32_t)time(nullptr));
	static std::uniform_real_distribution<float32_t> s_Distribution(0.0f, 1.0f);

	return Math::Vec4(s_Distribution(s_Engine), s_Distribution(s_Engine), s_Distribution(s_Engine), 1.0f);
	/*
		std::random_device rd;
		return std::bind(
			std::uniform_real_distribution<>{from, to},
			std::default_random_engine{ rd() })();
	*/
}

namespaceEnd(Color)
namespaceEnd(Gear)
