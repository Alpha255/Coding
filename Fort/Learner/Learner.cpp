#include "../../Gear/String.hpp"

int main()
{
	std::string test("sdlfjsldfjlsdfjlSDFSDFSDFSDFsdjflsdjflsdjfsdkf");
	Gear::String::toLower(test);
	Gear::String::replace(test, "s", "ADA");
	Gear::String::replace(test, "ADA", "E");

	auto result = Gear::String::split(test, "E");

	auto str = Gear::String::format("this is a %d", 2);

	return 0;
}