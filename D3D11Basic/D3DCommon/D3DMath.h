#pragma once

#include "Common.h"
#include <DirectXMath.h>
#include <vector>

class Vec2 : public DirectX::XMFLOAT2
{
public:
	inline Vec2() {}

	inline Vec2(float x, float y)
		: DirectX::XMFLOAT2(x, y)
	{
	}

	inline Vec2(const float* pArray)
		: DirectX::XMFLOAT2(pArray)
	{
	}
};

class Vec3 : public DirectX::XMFLOAT3
{
public:
	inline Vec3() {}

	inline Vec3(float x, float y, float z)
		: DirectX::XMFLOAT3(x, y, z)
	{
	}

	inline Vec3(const float* pArray)
		: DirectX::XMFLOAT3(pArray)
	{
	}

	inline friend Vec3 operator-(const Vec3 &left, const Vec3 &right)
	{
		return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
	}
};

class Matrix : public DirectX::XMMATRIX
{
public:
	inline Matrix()
	{
	}

	inline Matrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: DirectX::XMMATRIX(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{
	}

	inline Matrix(const float* pArray)
		: DirectX::XMMATRIX(pArray)
	{
	}

	inline Matrix Transpose()
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(*this);

		return *(static_cast<Matrix*>(&result));
	}

	inline Matrix Inverse()
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(nullptr, *this);

		return *(static_cast<Matrix*>(&result));
	}

	inline Matrix InverseTranspose()
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		Matrix temp = *this;
		temp.r[3] = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(temp);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, *this));

		return *(static_cast<Matrix*>(&result));
	}

	inline void Identity()
	{
		static const DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		memcpy(this, &I, sizeof(Matrix));
	}

	inline static Matrix LookAtLH(const Vec3& eyePos, const Vec3& lookAt, const Vec3& upDir)
	{
		DirectX::XMVECTOR eye = DirectX::XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		DirectX::XMVECTOR look = DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(upDir.x, upDir.y, upDir.z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixLookAtLH(eye, look, up);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix PerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);
		
		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix Translation(float x, float y, float z)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixTranslation(x, y, z);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix Scaling(float x, float y, float z)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixScaling(x, y, z);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix RotationAxis(float x, float y, float z, float angle)
	{
		DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixRotationAxis(axis, angle);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix Reflect(float x, float y, float z)
	{
		DirectX::XMVECTOR plane = DirectX::XMVectorSet(x, y, z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixReflect(plane);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix Shadow(float px, float py, float pz, float lx, float ly, float lz)
	{
		DirectX::XMVECTOR p = DirectX::XMVectorSet(px, py, pz, 0.0f);
		DirectX::XMVECTOR lit = DirectX::XMVectorSet(lx, ly, lz, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixShadow(p, lit);

		return *(static_cast<Matrix*>(&result));
	}

	inline static Matrix Shadow(const class Vec4 &plane, const class Vec4 &litDir);

	inline Matrix operator*(const Matrix& matrix) const
	{
		DirectX::XMMATRIX lMatrix, rMatrix;
		memcpy(&lMatrix, this, sizeof(Matrix));
		memcpy(&rMatrix, &matrix, sizeof(Matrix));

		DirectX::XMMATRIX result = lMatrix * rMatrix;

		return *(static_cast<Matrix*>(&result));
	}
};

class Vec4 : public DirectX::XMFLOAT4
{
public:
	inline Vec4() {}

	inline Vec4(float x, float y, float z, float w)
		: DirectX::XMFLOAT4(x, y, z, w)
	{
	}

	inline Vec4(const float* pArray)
		: DirectX::XMFLOAT4(pArray)
	{
	}

	inline void Transform(const class Matrix& matrix)
	{
		DirectX::XMVECTOR srcVec = DirectX::XMVectorSet(x, y, z, w);

		DirectX::XMVECTOR result = DirectX::XMVector3Transform(srcVec, matrix);

		memcpy(this, &result, sizeof(DirectX::XMVECTOR));
	}

	inline void TransformNormal(const class Matrix& matrix)
	{
		DirectX::XMVECTOR srcVec = DirectX::XMVectorSet(x, y, z, w);

		DirectX::XMVECTOR result = DirectX::XMVector3TransformNormal(srcVec, matrix);

		memcpy(this, &result, sizeof(DirectX::XMVECTOR));
	}

	friend inline Vec4 operator* (const Vec4 &left, float factor)
	{
		Vec4 result = left;
		result.x *= factor;
		result.y *= factor;
		result.z *= factor;
		result.w *= factor;

		return result;
	}
};

inline Matrix Matrix::Shadow(const Vec4 &plane, const Vec4 &litDir)
{
	DirectX::XMVECTOR p = DirectX::XMVectorSet(plane.x, plane.y, plane.z, plane.w);
	DirectX::XMVECTOR lit = DirectX::XMVectorSet(litDir.x, litDir.y, litDir.z, 0.0f);

	DirectX::XMMATRIX result = DirectX::XMMatrixShadow(p, lit);

	return *(static_cast<Matrix*>(&result));
}

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

NamespaceBegin(Geometry)
struct BasicVertex 
{
	Vec3 Position;
	Vec3 Normal;
	Vec2 UV;

	BasicVertex()
	{
		memset(this, 0, sizeof(BasicVertex));
	}
};

struct Vertex 
{
	Vec3 Position;
	Vec3 Normal;
	Vec3 Tangent;
	Vec2 UV;

	Vertex() 
	{
		memset(this, 0, sizeof(Vertex));
	}
	Vertex(const Vec3& pos, const Vec3& normal, const Vec3& tangent, const Vec2& uv)
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
	inline const Vec3& operator[](int i) const { return m_pCurSolution[i]; }
	inline const Vec3& Normal(int index) const { return m_pNormals[index]; }
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

	Vec3* m_pPrevSolution;
	Vec3* m_pCurSolution;
	Vec3* m_pNormals;
};

void MakeBox(float width, float height, float depth, Mesh& mesh);
void MakeCube(float width, Mesh& mesh);
void MakeSphere(float radius, uint32_t slice, uint32_t stack, Mesh& mesh);
void MakeGeoSphere(float radius, uint32_t subDivisions, Mesh& mesh);
void MakeFlatGeoSphere(float radius, uint32_t subDivisions, Mesh& mesh);
void MakeCylinder(float bottomRadius, float topRadius, float height, uint32_t slice, uint32_t stack, Mesh& mesh);
void MakeGrid(float width, float depth, uint32_t m, uint32_t n, Mesh& mesh);
void MakeQuad(Mesh& mesh);
void SubDivide(Mesh& mesh);
NamespaceEnd(Geometry)
NamespaceEnd(Math)

NamespaceBegin(Color)
XMGLOBALCONST Vec4 White = { 1.0f, 1.0f, 1.0f, 1.0f };
XMGLOBALCONST Vec4 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
XMGLOBALCONST Vec4 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
XMGLOBALCONST Vec4 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
XMGLOBALCONST Vec4 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
XMGLOBALCONST Vec4 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
XMGLOBALCONST Vec4 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
XMGLOBALCONST Vec4 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
XMGLOBALCONST Vec4 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
XMGLOBALCONST Vec4 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
XMGLOBALCONST Vec4 DarkBlue = { 0.0f, 0.125f, 0.3f, 1.0f };
NamespaceEnd(Color)

NamespaceBegin(Lighting)
struct DirectionalLight
{
	Vec4 Ambient;
	Vec4 Diffuse;
	Vec4 Specular;
	Vec4 Direction;  
};

struct PointLight 
{
	Vec4 Ambient;
	Vec4 Diffuse;
	Vec4 Specular;

	Vec3 Position;
	float Range;

	Vec4 Attenuation;
};

struct SpotLight 
{
	Vec4 Ambient;
	Vec4 Diffuse;
	Vec4 Specular;

	Vec3 Position;
	float Range;

	Vec3 Direction;
	float Spot;

	Vec4 Attenuation; /// padding with sizeof(float)
};

struct Material 
{
	Vec4 Ambient;
	Vec4 Diffuse;
	Vec4 Specular;
	Vec4 Reflect;
};
NamespaceEnd(Lighting)