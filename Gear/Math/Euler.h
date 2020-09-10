#pragma once

#include "Gear/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class Euler : public Vec4
{
public:
	void set(float32_t roll, float32_t pitch, float32_t yaw);
protected:
private:
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
