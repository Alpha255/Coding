#pragma once

#include "Vector4.h"

namespaceStart(gear)
namespaceStart(math)

class aabb
{
public:
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

	//inline vec3 getMin() const
	//{
	//	return m_Extents * vec3(-1.0f, -1.0f, 1.0f) + m_Center;
	//}

	//inline vec3 getMax() const
	//{
	//	return m_Extents * vec3(1.0f, 1.0f, 1.0f) + m_Center;
	//}

	inline vec3 getCenter() const
	{
		return m_Center;
	}

	inline vec3 getExtents() const
	{
		return m_Extents;
	}

	std::vector<vec3> getVertices() const;

	inline static aabb createFromVertices(const vec3 *pVertices, uint32_t vertexCount)
	{
		std::vector<vec3> vertices(vertexCount);
		memcpy(vertices.data(), pVertices, sizeof(vec3) * vertexCount);
		return createFromVertices(vertices);
	}
	static aabb createFromVertices(const std::vector<vec3> &vertices);
protected:
private:
	vec3 m_Center;
	vec3 m_Extents;
};

namespaceEnd(math)
namespaceEnd(gear)
