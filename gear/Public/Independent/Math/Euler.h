#pragma once

#include "Vector4.h"

namespaceStart(gear)
namespaceStart(math)

class euler : public vec4
{
public:
	void set(float32_t roll, float32_t pitch, float32_t head);
protected:
private:
};

namespaceEnd(math)
namespaceEnd(gear)
