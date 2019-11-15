#include "AABB.h"

namespaceStart(gear)
namespaceStart(math)

std::vector<vec3> aabb::getVertices() const
{
	static const vec3 s_Offsets[] = 
	{
		{ -1.0f, -1.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f },
		{ -1.0f, -1.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f },
	};

	std::vector<vec3> result(_countof(s_Offsets));
	for (uint32_t i = 0u; i < _countof(s_Offsets); ++i)
	{
		result[i] = m_Extents * s_Offsets[i] + m_Center;
	}

	return result;
}

aabb aabb::createFromVertices(const std::vector<vec3> &vertices)
{
	vec3 vMin, vMax;
	for (uint32_t i = 0u; i < vertices.size(); ++i)
	{
		vMin = getMin(vMin, vertices[i]);
		vMax = getMax(vMax, vertices[i]);
	}

	aabb result;
	result.m_Center = (vMin + vMax) * 0.5f;
	result.m_Extents = (vMax - vMin) * 0.5f;

	return result;
}

namespaceEnd(math)
namespaceEnd(gear)