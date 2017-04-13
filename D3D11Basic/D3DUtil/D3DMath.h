#pragma once

#include "Common.h"
#include <DirectXMath.h>
#include <vector>

NamespaceBegin(Math)
template<typename T> inline T Clamp(const T& x, const T& low, const T& high) 
{
	return x < low ? low : (x > high ? high : x);
}

/// Returns random float in [0, 1).
inline float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

/// Returns random float in [a, b).
inline float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

inline float AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	/// Quadrant I or IV
	if (x >= 0.0f)
	{
		/// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		///                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); /// in [-pi/2, +pi/2]

		if (theta < 0.0f)
		{
			theta += 2.0f * DirectX::XM_PI; /// in [0, 2*pi).
		}
	}
	/// Quadrant II or III
	else
	{
		theta = atanf(y / x) + DirectX::XM_PI; /// in [0, 2*pi).
	}

	return theta;
}

inline DirectX::XMMATRIX InverseTranspose(DirectX::CXMMATRIX M)
{
	/// Inverse-transpose is just applied to normals.  So zero out 
	/// translation row so that it doesn't get into our inverse-transpose
	/// calculation--we don't want the inverse-transpose of the translation.
	DirectX::XMMATRIX A = M;
	A.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(A);
	return DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, A));
}

NamespaceBegin(Geometry)
struct BasicVertex 
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 UV;
};

struct Vertex 
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT3 Tangent;
	DirectX::XMFLOAT2 UV;

	Vertex() {}
	Vertex(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangent, const DirectX::XMFLOAT2& uv)
		: Position(pos)
		, Normal(normal)
		, Tangent(tangent)
		, UV(uv) 
	{
	}
	Vertex(float px, float py, float pz,
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

struct Mesh
{
	std::vector<Vertex> Vertices;
	std::vector<uint32_t> Indices;
};

class Waves
{
public:
	Waves() 
		: m_Row(0U)
		, m_Column(0U)
		, m_VertexNum(0U)
		, m_TriangleNum(0U)
		, m_TimeStep(0.0f)
		, m_SpatialStep(0.0f)
		, m_K1(0.0f)
		, m_K2(0.0f)
		, m_K3(0.0f)
		, m_pCurSolution(nullptr)
		, m_pPrevSolution(nullptr)
		, m_pNormals(nullptr)
	{
	}
	~Waves() 
	{
		SafeDeleteArray(m_pPrevSolution);
		SafeDeleteArray(m_pCurSolution);
		SafeDeleteArray(m_pNormals);
	}

	inline uint32_t Row() const { return m_Row; }
	inline uint32_t Column() const { return m_Column; }
	inline uint32_t VertexCount() const { return m_VertexNum; }
	inline uint32_t TriangleCount() const { return m_TriangleNum; }
	inline const DirectX::XMFLOAT3& operator[](int i) const { return m_pCurSolution[i]; }
	inline const DirectX::XMFLOAT3& Normal(int index) const { return m_pNormals[index]; }
	inline float Width() const { return m_Column * m_SpatialStep; }
	inline float Depth() const { return m_Row * m_SpatialStep; }

	void Create(uint32_t row, uint32_t col, float spatialStep, float timeStep, float speed, float damping);
	void Update(float fElapsed);
	void Disturb(uint32_t i, uint32_t j, float magnitude);
protected:
private:
	uint32_t m_Row;
	uint32_t m_Column;
	uint32_t m_VertexNum;
	uint32_t m_TriangleNum;

	float m_TimeStep;
	float m_SpatialStep;

	float m_K1;
	float m_K2;
	float m_K3;

	DirectX::XMFLOAT3* m_pPrevSolution;
	DirectX::XMFLOAT3* m_pCurSolution;
	DirectX::XMFLOAT3* m_pNormals;
};

void MakeBox(float width, float height, float depth, Mesh& mesh);
void MakeCube(float width, Mesh& mesh);
void MakeSphere(float radius, uint32_t slice, uint32_t stack, Mesh& mesh);
void MakeGeoSphere(float radius, uint32_t subDivisions, Mesh& mesh);
void MakeCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack, Mesh& mesh);
void MakeGrid(float width, float depth, uint32_t m, uint32_t n, Mesh& mesh);
void MakeQuad(Mesh& mesh);
void SubDivide(Mesh& mesh);
NamespaceEnd(Geometry)
NamespaceEnd(Math)

NamespaceBegin(Color)
XMGLOBALCONST DirectX::XMFLOAT4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
XMGLOBALCONST DirectX::XMFLOAT4 DarkBlue = { 0.0f, 0.125f, 0.3f, 1.0f };
NamespaceEnd(Color)

NamespaceBegin(Light)
struct DirectionalLight
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT4 Direction;  /// padding with sizeof(float)
};

struct PointLight 
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT4 Attenuation; /// padding with sizeof(float)
};

struct SpotLight 
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;

	DirectX::XMFLOAT3 Position;
	float Range;

	DirectX::XMFLOAT3 Direction;
	float Spot;

	DirectX::XMFLOAT4 Attenuation; /// padding with sizeof(float)
};

struct Material 
{
	DirectX::XMFLOAT4 Ambient;
	DirectX::XMFLOAT4 Diffuse;
	DirectX::XMFLOAT4 Specular;
	DirectX::XMFLOAT4 Reflect;
};
NamespaceEnd(Light)