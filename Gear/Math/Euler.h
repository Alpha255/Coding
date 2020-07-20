#pragma once

#include "Vector4.h"

namespaceStart(Gear)
namespaceStart(Math)

class Euler : public Vec4
{
public:
	void set(float32_t roll, float32_t pitch, float32_t yaw);
protected:
private:
};

namespaceEnd(Math)
namespaceEnd(Gear)
