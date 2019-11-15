#pragma once

#include "Vector4.h"

namespaceStart(gear)
namespaceStart(math)

class plane : public vec4
{
public:
	plane()
		: vec4(0.0f, 1.0f, 0.0f, 0.0f)
	{
	}

	plane(const vec3 &normal, float32_t distance)
		: vec4(normal, distance)
	{
	}

	plane(const vec3 &vertex0, const vec3 &vertex1, const vec3 &vertex2)
	{
		vec3 v10 = vertex0 - vertex1;
		vec3 v20 = vertex0 - vertex2;
		v10.cross(v20);

		vec3 normal(v10);
		normal.normalize();

		plane result(normal, -normal.dot(vertex1));
		*this = result;
	}

	plane(const vec3 &vertex, const vec3 &normal)
	{
		plane result(normal, -normal.dot(vertex));
		*this = result;
	}

	inline vec3 normal() const
	{
		return vec3(x, y, z);
	}

	inline float32_t distance() const
	{
		return w;
	}
protected:
private:
};

namespaceEnd(math)
namespaceEnd(gear)
