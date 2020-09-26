#include "../../Gear/Log.hpp"
#include "../../Gear/System.h"
#include "../../Gear/IO/IO.h"

int main()
{
	std::string test("sdlfjsldfjlsdfjlSDFSDFSDFSDFsdjflsdjflsdjfsdkf");
	Gear::String::toLower(test);
	Gear::String::replace(test, "s", "ADA");
	Gear::String::replace(test, "ADA", "E");

	auto result = Gear::String::split(test, "E");

	auto str = Gear::String::format("this is a %d", 2);

	LOG_ERROR("This is a test %d.", 2);

	LOG_WARNING("This is a test %.2f.", 2.0f);

	Gear::SyncContinuousOFStream writer("test.txt", Gear::File::EMode::Text);
	writer.write(12.0f);
	writer.write("12.0f");

	LOG_INFO("Current Working Directory: %s", Gear::System::getCurrentWorkingDirectory().c_str());

	return 0;
}