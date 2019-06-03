#pragma once

#include "Base/Base.h"

NamespaceBegin(Geometry)

struct VertexLayout
{
	std::string Semantic;
	size_t Stride = 0U;
	uint32_t Offset = 0U;
	uint32_t Format = 0U;
};

struct Vertex
{
	Vec3 Position = {};
	Vec3 Normal = {};
	Vec3 Tangent = {};
	Vec2 UV = {};

	Vertex() = default;

	Vertex(const Vec3 &pos, const Vec3 &normal, const Vec3 &tangent, const Vec2 &uv)
		: Position(pos)
		, Normal(normal)
		, Tangent(tangent)
		, UV(uv)
	{
	}

	Vertex(
		float px, float py, float pz,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float u, float v)
		: Position(px, py, pz)
		, Normal(nx, ny, nz)
		, Tangent(tx, ty, tz)
		, UV(u, v)
	{
	}
};

class Box
{
public:
	Box() = default;
	Box(const Vec3 &min, const Vec3 &max);

	inline Vec3 GetCenter() const
	{
		return m_Center;
	}

	inline Vec3 GetSize() const
	{
		return m_Size;
	}
protected:
private:
	Vec3 m_Center;
	Vec3 m_Min;
	Vec3 m_Max;
	Vec3 m_Size;

	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;
};

NamespaceEnd(Geometry)
