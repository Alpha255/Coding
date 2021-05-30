#include "Gear/Math/Color.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);
Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
Color Color::Cyan(0.0f, 1.0f, 1.0f, 1.0f);
Color Color::Magenta(1.0f, 0.0f, 1.0f, 1.0f);
Color Color::Silver(0.75f, 0.75f, 0.75f, 1.0f);
Color Color::LightSteelBlue(0.69f, 0.77f, 0.87f, 1.0f);
Color Color::DarkBlue(0.0f, 0.125f, 0.3f, 1.0f);

Color Color::random()
{
	static std::default_random_engine s_Engine((uint32_t)time(nullptr));
	static std::uniform_real_distribution<float32_t> s_Distribution(0.0f, 1.0f);

	return Color(s_Distribution(s_Engine), s_Distribution(s_Engine), s_Distribution(s_Engine), 1.0f);
	/*
		std::random_device rd;
		return std::bind(
			std::uniform_real_distribution<>{from, to},
			std::default_random_engine{ rd() })();
	*/
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)