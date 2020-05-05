#pragma once

#include "Vector4.h"

namespaceStart(Gear)
namespaceStart(Math)

class Plane : public Vec4
{
public:
	Plane()
		: Vec4(0.0f, 1.0f, 0.0f, 0.0f)
	{
	}

	Plane(const Vec3 &normal, float32_t distance)
		: Vec4(normal, distance)
	{
	}

	Plane(const Vec3 &vertex0, const Vec3 &vertex1, const Vec3 &vertex2)
	{
		Vec3 v10 = vertex0 - vertex1;
		Vec3 v20 = vertex0 - vertex2;
		v10.cross(v20);

		Vec3 normal(v10);
		normal.normalize();

		Plane result(normal, -normal.dot(vertex1));
		*this = result;
	}

	Plane(const Vec3 &vertex, const Vec3 &normal)
	{
		Plane result(normal, -normal.dot(vertex));
		*this = result;
	}

	inline Vec3 normal() const
	{
		return Vec3(x, y, z);
	}

	inline float32_t distance() const
	{
		return w;
	}
protected:
private:
};

namespaceEnd(Math)
namespaceEnd(Gear)
