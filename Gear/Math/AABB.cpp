#include "AABB.h"

namespaceStart(Gear)
namespaceStart(Math)

std::vector<Vec3> AABB::vertices() const
{
	static const Vec3 s_Offsets[] = 
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

	std::vector<Vec3> result(_countof(s_Offsets));
	for (uint32_t i = 0u; i < _countof(s_Offsets); ++i)
	{
		result[i] = m_Extents * s_Offsets[i] + m_Center;
	}

	return result;
}

AABB AABB::createFromVertices(const std::vector<Vec3> &vertices)
{
	Vec3 vMin, vMax;
	for (uint32_t i = 0u; i < vertices.size(); ++i)
	{
		vMin = getMin(vMin, vertices[i]);
		vMax = getMax(vMax, vertices[i]);
	}

	AABB result;
	result.m_Center = (vMin + vMax) * 0.5f;
	result.m_Extents = (vMax - vMin) * 0.5f;

	return result;
}

namespaceEnd(Math)
namespaceEnd(Gear)
