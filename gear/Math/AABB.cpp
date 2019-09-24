#include "AABB.h"

namespaceStart(gear)
namespaceStart(math)

std::array<vec3, aabb::eVertexCount> aabb::getVertices() const
{
	static vec3 s_Offsets[eVertexCount] = 
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

	std::array<vec3, eVertexCount> result;
	for (uint32_t i = 0u; i < eVertexCount; ++i)
	{
		result[i] = m_Extents * s_Offsets[i] + m_Center;
	}

	return result;
}

aabb aabb::createFromVertices(const vec3 *pVertices, uint32_t vertexCount)
{
	assert(pVertices && vertexCount);
	
	vec3 vMin, vMax;
	for (uint32_t i = 0u; i < vertexCount; ++i)
	{
		vMin = getMin(vMin, pVertices[i]);
		vMax = getMax(vMax, pVertices[i]);
	}

	aabb result;
	result.m_Center = (vMin + vMax) * 0.5f;
	result.m_Extents = (vMax - vMin) * 0.5f;

	return result;
}

namespaceEnd(math)
namespaceEnd(gear)