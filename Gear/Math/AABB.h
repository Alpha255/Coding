#pragma once

#include "Gear/Math/Vector4.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

class AABB
{
public:
	AABB()
		: m_Center(0.0f, 0.0f, 0.0f)
		, m_Extents(1.0f, 1.0f, 1.0f)
	{
	}

	AABB(const Vec3& center, const Vec3& extents)
		: m_Center(center)
		, m_Extents(extents)
	{
	}

	inline Vec3 min() const
	{
		return m_Center - m_Extents;
	}

	inline Vec3 max() const
	{
		return m_Extents + m_Center;
	}

	inline Vec3 center() const
	{
		return m_Center;
	}

	inline Vec3 extents() const
	{
		return m_Extents;
	}

	std::vector<Vec3> vertices() const;
	std::vector<uint32_t> indices() const
	{
		auto indices = std::vector<uint32_t>
		{
			0, 1, 2, 0, 2, 3,  
			4, 5, 6, 4, 6, 7,
			1, 0, 4, 1, 4, 5,
			3, 2, 6, 3, 6, 7,
			0, 3, 4, 0, 7, 4,
			1, 2, 5, 1, 6, 5
		};
		return indices;
	}

	inline static AABB createFromVertices(const Vec3* srcvertices, uint32_t count)
	{
		std::vector<Vec3> vertices(count);
		VERIFY(memcpy_s(vertices.data(), sizeof(Vec3) * count, srcvertices, sizeof(Vec3) * count) == 0);
		return createFromVertices(vertices);
	}
	static AABB createFromVertices(const std::vector<Vec3>& vertices);
protected:
private:
	Vec3 m_Center;
	Vec3 m_Extents;
};

NAMESPACE_END(Math)
NAMESPACE_END(Gear)
