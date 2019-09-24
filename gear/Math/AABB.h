#pragma once

#include "Vector4.h"
#include "Functions.h"

namespaceStart(gear)
namespaceStart(math)

class aabb
{
public:
	enum
	{
		eVertexCount = 8
	};

	aabb()
		: m_Center(0.0f, 0.0f, 0.0f)
		, m_Extents(1.0f, 1.0f, 1.0f)
	{
	}

	aabb(const vec3 &center, const vec3 &extents)
		: m_Center(center)
		, m_Extents(extents)
	{
	}

	std::array<vec3, eVertexCount> getVertices() const;
	static aabb createFromVertices(const vec3 *pVertices, uint32_t vertexCount);
protected:
private:
	vec3 m_Center;
	vec3 m_Extents;
};

namespaceEnd(math)
namespaceEnd(gear)
