#pragma once

#include "Gear/Definitions.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

#define USE_SSE
#define FORCE_ALIGN_16

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

/***************************************************************************************
	XMLoadFloat4A _mm_load_ps : Loads four f32 values from aligned memory into a __m128. 
								If the pointer is not aligned to a 128-bit boundary 
								(16 bytes) a general protection fault will be triggered 
								(fatal program crash). Use _mm_loadu_ps for potentially 
								unaligned memory.
	XMLoadFloat4  _mm_loadu_ps: Loads four f32 values from memory into a __m128. 
								There are no restrictions on memory alignment. 
								For aligned memory _mm_load_ps may be faster.
****************************************************************************************/

#if defined(FORCE_ALIGN_16)
	#define VECTOR_LOAD(Dimension, Vec) DirectX::XMLoadFloat##Dimension##A(Vec)
	#define VECTOR_STORE(Dimension, Dst, Src) DirectX::XMStoreFloat##Dimension##A(Dst, Src)
	#define MATRIX_LOAD(Matrix) DirectX::XMLoadFloat4x4A(Matrix)
	#define MATRIX_STORE(Dst, Src) DirectX::XMStoreFloat4x4A(Dst, Src)
	
	using Float2 = DirectX::XMFLOAT2A;
	using Float3 = DirectX::XMFLOAT3A;
	using Float4 = DirectX::XMFLOAT4A;
	using Float4x4 = DirectX::XMFLOAT4X4A;
#else
	#define VECTOR_LOAD(Dimension, Vec) DirectX::XMLoadFloat##Dimension(Vec)
	#define VECTOR_STORE(Dimension, Dst, Src) DirectX::XMStoreFloat##Dimension(Dst, Src)
	#define MATRIX_LOAD(Matrix) DirectX::XMLoadFloat4x4(Matrix)
	#define MATRIX_STORE(Dst, Src) DirectX::XMStoreFloat4x4(Dst, Src)

	using Float2 = DirectX::XMFLOAT2;
	using Float3 = DirectX::XMFLOAT3;
	using Float4 = DirectX::XMFLOAT4;
	using Float4x4 = DirectX::XMFLOAT4X4;
#endif

#define VECTOR_EQUAL(Dimension, Left, Right)               DirectX::XMVector##Dimension##Equal(Left, Right)
#define VECTOR_NOT_EQUAL(Dimension, Left, Right)           DirectX::XMVector##Dimension##NotEqual(Left, Right)
#define VECTOR_GREATER(Dimension, Left, Right)             DirectX::XMVector##Dimension##Greater(Left, Right)
#define VECTOR_GREATER_OR_EQUAL(Dimension, Left, Right)    DirectX::XMVector##Dimension##GreaterOrEqual(Left, Right)
#define VECTOR_LESS(Dimension, Left, Right)                DirectX::XMVector##Dimension##Less(Left, Right)
#define VECTOR_LESS_OR_EQUAL(Dimension, Left, Right)       DirectX::XMVector##Dimension##LessOrEqual(Left, Right)
#define VECTOR_CROSS(Dimension, Left, Right)               DirectX::XMVector##Dimension##Cross(Left, Right)
#define VECTOR_DOT(Dimension, Left, Right)                 DirectX::XMVector##Dimension##Dot(Left, Right)
#define VECTOR_IS_NAN(Dimension, Vec)                      DirectX::XMVector##Dimension##IsNaN(Vec)
#define VECTOR_IS_INFINITE(Dimension, Vec)                 DirectX::XMVector##Dimension##IsInfinite(Vec)
#define VECTOR_LENGTH(Dimension, Vec)                      DirectX::XMVector##Dimension##Length(Vec)
#define VECTOR_LENGTH_EST(Dimension, Vec)                  DirectX::XMVector##Dimension##LengthEst(Vec)  /// Est functions offer increased performance at the expense of reduced accuracy
#define VECTOR_RECIPROCAL_LENGTH(Dimension, Vec)           DirectX::XMVector##Dimension##ReciprocalLength(Vec)
#define VECTOR_RECIPROCAL_LENGTH_EST(Dimension, Vec)       DirectX::XMVector##Dimension##ReciprocalLengthEst(Vec)
#define VECTOR_LENGTH_SQ(Dimension, Vec)                   DirectX::XMVector##Dimension##LengthSq(Vec)
#define VECTOR_NORMALIZE(Dimension, Vec)                   DirectX::XMVector##Dimension##Normalize(Vec)
#define VECTOR_CLAMP_LENGTH(Dimension, Vec, Min, Max)      DirectX::XMVector##Dimension##ClampLength(Vec, Min, Max)
#define VECTOR_REFLECT(Dimension, Vec, Normal)             DirectX::XMVector##Dimension##Reflect(Vec, Normal)
#define VECTOR_REFRACT(Dimension, Vec, Normal, Index)      DirectX::XMVector##Dimension##Refract(Vec, Normal, Index)
#define VECTOR_ORTHOGONAL(Dimension, Vec)                  DirectX::XMVector##Dimension##Orthogonal(Vec)
#define VECTOR_TRANSFORM(Dimension, Vec, Trans)            DirectX::XMVector##Dimension##Transform(Vec, Trans)
#define VECTOR_TRANSFORM_COORD(Dimension, Vec, Trans)      DirectX::XMVector##Dimension##TransformCoord(Vec, Trans)
#define VECTOR_TRANSFORM_NORMAL(Dimension, Vec, Trans)     DirectX::XMVector##Dimension##TransformNormal(Vec, Trans)

#define VECTOR_MEMBER_FUNCTIONS(Dimension)                                                                                                       \
inline void operator+=(const Vec##Dimension& right)                                                                                              \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorAdd(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                           \
}                                                                                                                                                \
inline bool8_t operator>(const Vec##Dimension& right) const                                                                                      \
{                                                                                                                                                \
	return VECTOR_GREATER(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                              \
}                                                                                                                                                \
inline bool8_t operator>=(const Vec##Dimension& right) const                                                                                     \
{                                                                                                                                                \
	return VECTOR_GREATER_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                     \
}                                                                                                                                                \
inline bool8_t operator<(const Vec##Dimension& right) const                                                                                      \
{                                                                                                                                                \
	return VECTOR_LESS(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                                 \
}                                                                                                                                                \
inline bool8_t operator<=(const Vec##Dimension& right) const                                                                                     \
{                                                                                                                                                \
	return VECTOR_LESS_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                        \
}                                                                                                                                                \
inline bool8_t operator==(const Vec##Dimension& right) const                                                                                     \
{                                                                                                                                                \
	return VECTOR_EQUAL(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                                \
}                                                                                                                                                \
inline bool8_t operator!=(const Vec##Dimension& right) const                                                                                     \
{                                                                                                                                                \
	return VECTOR_NOT_EQUAL(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right));                                            \
}                                                                                                                                                \
inline void operator-=(const Vec##Dimension& right)                                                                                              \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorSubtract(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                      \
}                                                                                                                                                \
inline void operator*=(const float32_t right)                                                                                                    \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, this), right));                                                  \
}                                                                                                                                                \
inline void operator*=(const Vec##Dimension& right)                                                                                              \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorMultiply(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                      \
}                                                                                                                                                \
inline void operator/=(const float32_t right)                                                                                                    \
{                                                                                                                                                \
	assert(std::fpclassify(right) != FP_ZERO);                                                                                                   \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, this), 1.0f / right));                                           \
}                                                                                                                                                \
inline void operator/=(const Vec##Dimension& right)                                                                                              \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorDivide(VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                        \
}                                                                                                                                                \
inline float32_t dot(const Vec##Dimension& right) const                                                                                          \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_DOT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                           \
}                                                                                                                                                \
inline void normalize()                                                                                                                          \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, VECTOR_NORMALIZE(Dimension, VECTOR_LOAD(Dimension, this)));                                                    \
}                                                                                                                                                \
inline void negate()                                                                                                                             \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, DirectX::XMVectorNegate(VECTOR_LOAD(Dimension, this)));                                                        \
}                                                                                                                                                \
inline float32_t length() const                                                                                                                  \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH(Dimension, VECTOR_LOAD(Dimension, this)));                                                        \
}                                                                                                                                                \
inline float32_t lengthSq() const                                                                                                                \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_SQ(Dimension, VECTOR_LOAD(Dimension, this)));                                                     \
}                                                                                                                                                \
inline float32_t lengthEst() const                                                                                                               \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, this)));                                                    \
}                                                                                                                                                \
inline float32_t reciprocalLength() const                                                                                                        \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH(Dimension, VECTOR_LOAD(Dimension, this)));                                             \
}                                                                                                                                                \
inline float32_t reciprocalLengthEst() const                                                                                                     \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, this)));                                         \
}                                                                                                                                                \
inline bool8_t isNaN() const                                                                                                                     \
{                                                                                                                                                \
	return VECTOR_IS_NAN(Dimension, VECTOR_LOAD(Dimension, this));                                                                               \
}                                                                                                                                                \
inline bool8_t isInfinite() const                                                                                                                \
{                                                                                                                                                \
	return VECTOR_IS_INFINITE(Dimension, VECTOR_LOAD(Dimension, this));                                                                          \
}                                                                                                                                                \
inline void clampLength(float32_t min, float32_t max)                                                                                            \
{                                                                                                                                                \
	VECTOR_STORE(Dimension, this, VECTOR_CLAMP_LENGTH(Dimension, VECTOR_LOAD(Dimension, this), min, max));                                       \
}                                                                                                                                                \
inline Vec##Dimension reflect(const Vec##Dimension& normal) const                                                                                \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFLECT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &normal)));                  \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension refract(const Vec##Dimension& normal, float32_t refractionIndex) const                                                     \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFRACT(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &normal), refractionIndex)); \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension orthogonal() const                                                                                                         \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_ORTHOGONAL(Dimension, VECTOR_LOAD(Dimension, this)));                                                \
	return Result;                                                                                                                               \
}

#define VECTOR_MEMBER_FUNCTIONS_CROSS(Dimension)                                                                                                 \
inline Vec##Dimension cross(const Vec##Dimension& right) const                                                                                   \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_CROSS(Dimension, VECTOR_LOAD(Dimension, this), VECTOR_LOAD(Dimension, &right)));                     \
	return Result;                                                                                                                               \
}                                                                                                                                                

#define VECTOR_MEMBER_FUNCTIONS_TRANSFORM_DECLARE(Dimension)   \
void transform(const class Matrix& trans);                     \
void transformNormal(const class Matrix& trans);               \
void transformCoord(const class Matrix& trans);                \

#define VECTOR_MEMBER_FUNCTIONS_TRANSFORM(Dimension)                                                                      \
void Vec##Dimension::transform(const Matrix& trans)                                                                       \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&trans)));        \
}                                                                                                                         \
void Vec##Dimension::transformNormal(const Matrix& trans)                                                                 \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM_NORMAL(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&trans))); \
}                                                                                                                         \
void Vec##Dimension::transformCoord(const Matrix& trans)                                                                  \
{                                                                                                                         \
	VECTOR_STORE(Dimension, this, VECTOR_TRANSFORM_COORD(Dimension, VECTOR_LOAD(Dimension, this), MATRIX_LOAD(&trans)));  \
}

#define VECTOR_GLOBAL_FUNCTIONS(Dimension)                                                                                                       \
inline Vec##Dimension operator+(const Vec##Dimension& left, const Vec##Dimension& right)                                                         \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorAdd(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                       \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension operator-(const Vec##Dimension &left, const Vec##Dimension &right)                                                         \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorSubtract(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                  \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension operator*(const Vec##Dimension& left, const float32_t right)                                                               \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, &left), right));                                              \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension operator*(const Vec##Dimension& left, const Vec##Dimension& right)                                                         \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMultiply(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                  \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension operator/(const Vec##Dimension& left, const float32_t right)                                                               \
{                                                                                                                                                \
	assert(std::fpclassify(right) != FP_ZERO);                                                                                                   \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorScale(VECTOR_LOAD(Dimension, &left), 1.0f / right));                                       \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension operator/(const Vec##Dimension& left, const Vec##Dimension& right)                                                         \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorDivide(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                    \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline bool8_t operator==(const Vec##Dimension& left, const Vec##Dimension& right)                                                               \
{                                                                                                                                                \
	return VECTOR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                               \
}                                                                                                                                                \
inline bool8_t operator!=(const Vec##Dimension& left, const Vec##Dimension& right)                                                               \
{                                                                                                                                                \
	return VECTOR_NOT_EQUAL(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                           \
}                                                                                                                                                \
inline bool8_t operator>(const Vec##Dimension& left, const Vec##Dimension& right)                                                                \
{                                                                                                                                                \
	return VECTOR_GREATER(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                             \
}                                                                                                                                                \
inline bool8_t operator>=(const Vec##Dimension& left, const Vec##Dimension& right)                                                               \
{                                                                                                                                                \
	return VECTOR_GREATER_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                    \
}                                                                                                                                                \
inline bool8_t operator<(const Vec##Dimension& left, const Vec##Dimension& right)                                                                \
{                                                                                                                                                \
	return VECTOR_LESS(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                                \
}                                                                                                                                                \
inline bool8_t operator<=(const Vec##Dimension& left, const Vec##Dimension& right)                                                               \
{                                                                                                                                                \
	return VECTOR_LESS_OR_EQUAL(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right));                                       \
}                                                                                                                                                \
inline float32_t dot(const Vec##Dimension& left, const Vec##Dimension& right)                                                                    \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_DOT(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                          \
}                                                                                                                                                \
inline Vec##Dimension normalize(const Vec##Dimension& vec)                                                                                       \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_NORMALIZE(Dimension, VECTOR_LOAD(Dimension, &vec)));                                                 \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension negate(Vec##Dimension& vec)                                                                                                \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorNegate(VECTOR_LOAD(Dimension, &vec)));                                                     \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline float32_t length(const Vec##Dimension& vec)                                                                                               \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH(Dimension, VECTOR_LOAD(Dimension, &vec)));                                                        \
}                                                                                                                                                \
inline float32_t lengthSq(const Vec##Dimension& vec)                                                                                             \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_SQ(Dimension, VECTOR_LOAD(Dimension, &vec)));                                                     \
}                                                                                                                                                \
inline float32_t lengthEst(const Vec##Dimension& vec)                                                                                            \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, &vec)));                                                    \
}                                                                                                                                                \
inline float32_t reciprocalLength(const Vec##Dimension& vec)                                                                                     \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH(Dimension, VECTOR_LOAD(Dimension, &vec)));                                             \
}                                                                                                                                                \
inline float32_t reciprocalLengthEst(const Vec##Dimension& vec)                                                                                  \
{                                                                                                                                                \
	return DirectX::XMVectorGetX(VECTOR_RECIPROCAL_LENGTH_EST(Dimension, VECTOR_LOAD(Dimension, &vec)));                                         \
}                                                                                                                                                \
inline bool8_t isNaN(const Vec##Dimension& vec)                                                                                                  \
{                                                                                                                                                \
	return VECTOR_IS_NAN(Dimension, VECTOR_LOAD(Dimension, &vec));                                                                               \
}                                                                                                                                                \
inline bool8_t isInfinite(const Vec##Dimension& vec)                                                                                             \
{                                                                                                                                                \
	return VECTOR_IS_INFINITE(Dimension, VECTOR_LOAD(Dimension, &vec));                                                                          \
}                                                                                                                                                \
inline Vec##Dimension clampLength(const Vec##Dimension& vec, float32_t min, float32_t max)                                                       \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_CLAMP_LENGTH(Dimension, VECTOR_LOAD(Dimension, &vec), min, max));                                    \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension reflect(const Vec##Dimension& vec, const Vec##Dimension& normal)                                                           \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFLECT(Dimension, VECTOR_LOAD(Dimension, &vec), VECTOR_LOAD(Dimension, &normal)));                  \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension refract(const Vec##Dimension& vec, const Vec##Dimension& normal, float32_t refractionIndex)                                \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_REFRACT(Dimension, VECTOR_LOAD(Dimension, &vec), VECTOR_LOAD(Dimension, &normal), refractionIndex)); \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension orthogonal(const Vec##Dimension& vec)                                                                                      \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_ORTHOGONAL(Dimension, VECTOR_LOAD(Dimension, &vec)));                                                \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension min(const Vec##Dimension& left, const Vec##Dimension& right)                                                               \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMin(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                       \
	return Result;                                                                                                                               \
}                                                                                                                                                \
inline Vec##Dimension max(const Vec##Dimension &left, const Vec##Dimension &right)                                                               \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, DirectX::XMVectorMax(VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                       \
	return Result;                                                                                                                               \
}

#define VECTOR_GLOBAL_FUNCTIONS_CROSS(Dimension)                                                                                                 \
inline Vec##Dimension cross(const Vec##Dimension& left, const Vec##Dimension& right)                                                             \
{                                                                                                                                                \
	Vec##Dimension Result;                                                                                                                       \
	VECTOR_STORE(Dimension, &Result, VECTOR_CROSS(Dimension, VECTOR_LOAD(Dimension, &left), VECTOR_LOAD(Dimension, &right)));                    \
	return Result;                                                                                                                               \
}

template <class T>
inline DirectX::XMVECTOR TVectorLoad(const T* vec)
{
	return DirectX::XMVECTOR();
}

template <>
inline DirectX::XMVECTOR TVectorLoad(const DirectX::XMFLOAT2* vec)
{
	return DirectX::XMLoadFloat2(vec);
}
template <>
inline DirectX::XMVECTOR TVectorLoad(const DirectX::XMFLOAT2A* vec)
{
	return DirectX::XMLoadFloat2A(vec);
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)

