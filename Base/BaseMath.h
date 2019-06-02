#pragma once

class Vec2 : public DirectX::XMFLOAT2
{
public:
	inline Vec2()
		: DirectX::XMFLOAT2(0.0f, 0.0f)
	{
	}

	inline Vec2(float x, float y)
		: DirectX::XMFLOAT2(x, y)
	{
	}

	inline Vec2(const float* pArray)
		: DirectX::XMFLOAT2(pArray)
	{
	}

	inline void operator+=(const Vec2 &right)
	{
		x += right.x;
		y += right.y;
	}

	inline void operator-=(const Vec2 &right)
	{
		x -= right.x;
		y -= right.y;
	}

	inline void operator-=(const float right)
	{
		x -= right;
		y -= right;
	}

	inline void operator*=(const float f)
	{
		x *= f;
		y *= f;
	}

	inline friend Vec2 operator*(const Vec2 &left, const float f)
	{
		return Vec2(left.x * f, left.y * f);
	}

	inline friend Vec2 operator+(const Vec2 &left, const Vec2 &right)
	{
		return Vec2(left.x + right.x, left.y + right.y);
	}

	inline friend Vec2 operator-(const Vec2 &left, const Vec2 &right)
	{
		return Vec2(left.x - right.x, left.y - right.y);
	}
};

class Vec3 : public DirectX::XMFLOAT3
{
public:
	inline Vec3()
		: DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	{
	}

	inline Vec3(float x, float y, float z)
		: DirectX::XMFLOAT3(x, y, z)
	{
	}

	inline Vec3(const float* pArray)
		: DirectX::XMFLOAT3(pArray)
	{
	}

	inline void operator+=(const Vec3 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
	}

	inline void operator-=(const Vec3 &right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
	}

	inline void operator-=(const float right)
	{
		x -= right;
		y -= right;
		z -= right;
	}

	inline void operator*=(const float f)
	{
		x *= f;
		y *= f;
		z *= f;
	}

	inline friend Vec3 operator+(const Vec3 &left, const Vec3 &right)
	{
		return Vec3(left.x + right.x, left.y + right.y, left.z + right.z);
	}

	inline friend Vec3 operator-(const Vec3 &left, const Vec3 &right)
	{
		return Vec3(left.x - right.x, left.y - right.y, left.z - right.z);
	}

	inline friend float operator*(const Vec3 &left, const Vec3 &right)
	{
		return (left.x * right.x + left.y * right.y + left.z * right.z);
	}

	inline friend Vec3 operator*(const Vec3 &left, float scale)
	{
		return Vec3(left.x * scale, left.y * scale, left.z * scale);
	}

	inline static Vec3 Cross(const Vec3 &v0, const Vec3 &v1)
	{
		DirectX::XMVECTOR xv0 = DirectX::XMLoadFloat3(&v0);

		DirectX::XMVECTOR xv1 = DirectX::XMLoadFloat3(&v1);

		Vec3 result;
		DirectX::XMStoreFloat3(&result, DirectX::XMVector3Cross(xv0, xv1));

		return result;
	}

	inline void Normalize()
	{
		DirectX::XMVECTOR tmp = DirectX::XMLoadFloat3(this);

		DirectX::XMVECTOR normalized = DirectX::XMVector3Normalize(tmp);

		DirectX::XMStoreFloat3(this, normalized);
	}

	inline float LengthSq()
	{
		DirectX::XMVECTOR tmp = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR lengthSq = DirectX::XMVector3LengthSq(tmp);

		Vec3 result;
		DirectX::XMStoreFloat3(&result, lengthSq);

		return result.x;
	}
};

class Vec4 : public DirectX::XMFLOAT4
{
public:
	inline Vec4() 
		: DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0)
	{
	}

	inline Vec4(float x, float y, float z, float w)
		: DirectX::XMFLOAT4(x, y, z, w)
	{
	}

	inline Vec4(const float* pArray)
		: DirectX::XMFLOAT4(pArray)
	{
	}

	inline void operator+=(const Vec4 &right)
	{
		x += right.x;
		y += right.y;
		z += right.z;
		w += right.w;
	}

	inline void operator-=(const Vec4 &right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;
		w -= right.w;
	}

	inline void operator*=(const float f)
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
	}

	inline bool operator==(const Vec4 &right)
	{
		return x == right.x && y == right.y && z == right.z && w == right.w;
	}

	inline bool operator!=(const Vec4 &right)
	{
		return x != right.x || y != right.y || z != right.z || w != right.w;
	}

	inline friend Vec4 operator+(const Vec4 &left, const Vec4 &right)
	{
		return Vec4(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w);
	}

	inline friend Vec4 operator-(const Vec4 &left, const Vec4 &right)
	{
		return Vec4(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w);
	}

	inline friend Vec4 operator*(const Vec4 &left, const float f)
	{
		return Vec4(left.x * f, left.y * f, left.z * f, left.w * f);
	}

	inline friend Vec4 operator*(const Vec4 &left, const Vec4 &right)
	{
		return Vec4(left.x * right.x, left.y * right.y, left.z * right.z, left.w * right.w);
	}

	inline static Vec4 Cross(const Vec4 &v0, const Vec4 &v1, const Vec4 &v2)
	{
		DirectX::XMVECTOR xv0 = DirectX::XMLoadFloat4(&v0);

		DirectX::XMVECTOR xv1 = DirectX::XMLoadFloat4(&v1);

		DirectX::XMVECTOR xv2 = DirectX::XMLoadFloat4(&v2);

		Vec4 result;
		DirectX::XMStoreFloat4(&result, DirectX::XMVector4Cross(xv0, xv1, xv2));

		return result;
	}

	inline void Normalize()
	{
		DirectX::XMVECTOR tmp = DirectX::XMLoadFloat4(this);

		DirectX::XMVECTOR normalized = DirectX::XMVector4Normalize(tmp);

		DirectX::XMStoreFloat4(this, normalized);
	}
};

class Matrix : public DirectX::XMFLOAT4X4A
{
public:
	inline Matrix()
	{
		this->Identity();
	}

	inline Matrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
		: DirectX::XMFLOAT4X4A(
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33)
	{
	}

	inline Matrix(const float *pArray)
		: DirectX::XMFLOAT4X4A(pArray)
	{
	}

	inline void Transpose()
	{
		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(target);
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline static Matrix Transpose(const Matrix &other)
	{
		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(&other);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(target);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline void Inverse()
	{
		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(nullptr, target);
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline static Matrix Inverse(const Matrix &other)
	{
		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(&other);
		DirectX::XMMATRIX result = DirectX::XMMatrixInverse(nullptr, target);
		
		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline void InverseTranspose()
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		_41 = 0.0f;
		_42 = 0.0f;
		_43 = 0.0f;
		_44 = 1.0f;

		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(target);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, target));
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline static Matrix InverseTranspose(const Matrix &other)
	{
		/// Inverse-transpose is just applied to normals.  So zero out 
		/// translation row so that it doesn't get into our inverse-transpose
		/// calculation--we don't want the inverse-transpose of the translation.
		Matrix copy = other;
		copy._41 = 0.0f;
		copy._42 = 0.0f;
		copy._43 = 0.0f;
		copy._44 = 1.0f;

		DirectX::XMMATRIX target = DirectX::XMLoadFloat4x4A(&copy);
		DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(target);
		DirectX::XMMATRIX result = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, target));
		
		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline void Identity()
	{
		DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
		DirectX::XMStoreFloat4x4A(this, I);
	}

	inline static Matrix IdentityMatrix()
	{
		DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, I);

		return returnValue;
	}

	inline static Matrix LookAtLH(const Vec3 &eyePos, const Vec3 &lookAt, const Vec3 &upDir)
	{
		DirectX::XMVECTOR eye = DirectX::XMVectorSet(eyePos.x, eyePos.y, eyePos.z, 1.0f);
		DirectX::XMVECTOR look = DirectX::XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(upDir.x, upDir.y, upDir.z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixLookAtLH(eye, look, up);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix PerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearPlane, farPlane);
		
		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix OrthographicLH(float width, float height, float nearPlane, float farPlane)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixOrthographicLH(width, height, nearPlane, farPlane);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Translation(float x, float y, float z)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixTranslation(x, y, z);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Scaling(float x, float y, float z)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixScaling(x, y, z);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Scaling(float f)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixScaling(f, f, f);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix RotationAxis(float x, float y, float z, float angle)
	{
		DirectX::XMVECTOR axis = DirectX::XMVectorSet(x, y, z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixRotationAxis(axis, angle * DirectX::XM_PI / 180.0f);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Reflect(float x, float y, float z)
	{
		DirectX::XMVECTOR plane = DirectX::XMVectorSet(x, y, z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixReflect(plane);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Reflect(const Vec4 &plane)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixReflect(DirectX::XMLoadFloat4(&plane));

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Shadow(float px, float py, float pz, float lx, float ly, float lz)
	{
		DirectX::XMVECTOR p = DirectX::XMVectorSet(px, py, pz, 0.0f);
		DirectX::XMVECTOR lit = DirectX::XMVectorSet(lx, ly, lz, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixShadow(p, lit);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix Shadow(const Vec4 &plane, const Vec4 &litDir)
	{
		DirectX::XMVECTOR p = DirectX::XMVectorSet(plane.x, plane.y, plane.z, plane.w);
		DirectX::XMVECTOR lit = DirectX::XMVectorSet(litDir.x, litDir.y, litDir.z, 0.0f);

		DirectX::XMMATRIX result = DirectX::XMMatrixShadow(p, lit);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline static Matrix RotateRollPitchYaw(float pitch, float yaw, float roll)
	{
		DirectX::XMMATRIX result = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}

	inline void operator*=(const Matrix &right)
	{
		DirectX::XMMATRIX lMatrix = DirectX::XMLoadFloat4x4A(this);
		DirectX::XMMATRIX rMatrix = DirectX::XMLoadFloat4x4A(&right);
		DirectX::XMMATRIX result = lMatrix * rMatrix;
		DirectX::XMStoreFloat4x4A(this, result);
	}

	inline friend Matrix operator*(const Matrix &left, const Matrix &right)
	{
		DirectX::XMMATRIX lMatrix = DirectX::XMLoadFloat4x4A(&left);
		DirectX::XMMATRIX rMatrix = DirectX::XMLoadFloat4x4A(&right);
		DirectX::XMMATRIX result = lMatrix * rMatrix;
		
		Matrix returnValue;
		DirectX::XMStoreFloat4x4A(&returnValue, result);

		return returnValue;
	}
};

inline Vec3 TransformCoord(const Vec3 &v, const Matrix &m)
{
	DirectX::XMVECTOR tmpV = DirectX::XMLoadFloat3(&v);
	DirectX::XMMATRIX tmpM = DirectX::XMLoadFloat4x4A(&m);

	DirectX::XMVECTOR tmpResult = DirectX::XMVector3TransformCoord(tmpV, tmpM);

	Vec3 result;
	DirectX::XMStoreFloat3(&result, tmpResult);

	return result;
}

NamespaceBegin(Math)

using namespace DirectX;

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
		theta = ::atanf(y / x); /// in [-pi/2, +pi/2]

		if (theta < 0.0f)
		{
			theta += 2.0f * DirectX::XM_PI; /// in [0, 2*pi).
		}
	}
	/// Quadrant II or III
	else
	{
		theta = ::atanf(y / x) + DirectX::XM_PI; /// in [0, 2*pi).
	}

	return theta;
}

inline Vec4 GetPlaneFromPointNormal(const Vec3 &point, const Vec3 &normal)
{
	DirectX::XMVECTOR pointVec = DirectX::XMVectorSet(point.x, point.y, point.z, 0.0f);
	DirectX::XMVECTOR normalVec = DirectX::XMVectorSet(normal.x, normal.y, normal.z, 0.0f);

	DirectX::XMVECTOR plane = DirectX::XMPlaneFromPointNormal(pointVec, normalVec);

	Vec4 result;
	DirectX::XMStoreFloat4(&result, plane);

	return result;
}

inline float PlaneDotCoord(const Vec4 &plane, const Vec4 &coord)
{
	DirectX::XMVECTOR planeVec = DirectX::XMVectorSet(plane.x, plane.y, plane.z, plane.w);
	DirectX::XMVECTOR coordVec = DirectX::XMVectorSet(coord.x, coord.y, coord.z, coord.w);
	DirectX::XMVECTOR dot = DirectX::XMPlaneDotCoord(planeVec, coordVec);

	float result;
	DirectX::XMStoreFloat(&result, dot);

	return result;
}

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