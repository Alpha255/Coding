#pragma once

#include "Vector4.h"
#include "Functions.h"

namespaceStart(gear)
namespaceStart(math)

class euler
{
public:
	void set(float32_t roll, float32_t pitch, float32_t head);
protected:
private:
	vec4 m_RollPitchHead;
};

namespaceEnd(math)
namespaceEnd(gear)
