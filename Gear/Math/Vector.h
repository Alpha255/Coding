#pragma once

#include "Gear/Definitions.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

#define USE_SSE

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

#define VECTOR_NORMALIZE(Dimension, Vec)                                      \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Normalize(temp);\
	DirectX::XMStoreFloat##Dimension(Vec, vResult);                           \
}
#define VECTOR_ADD(Dimension, VecLeft, VecRight, VecResult)                   \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define VECTOR_SUBTRACT(Dimension, VecLeft, VecRight, VecResult)              \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	v1 = DirectX::XMVectorNegate(v1);                                         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define VECTOR_MULTIPLY(Dimension, VecLeft, VecRight, VecResult)              \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMultiply(v0, v1);            \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define VECTOR_CROSS(Dimension, VecLeft, VecRight, VecResult)                 \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector3Cross(v0, v1);              \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define VECTOR_SCALE(Dimension, Vec, Factor, VecResult)                       \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVectorScale(temp, Factor);         \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define VECTOR_DOT(Dimension, VecLeft, VecRight, Result)                      \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Dot(v0, v1);    \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}
#define VECTOR_LENGTH(Dimension, Vec, Result)                                 \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Length(temp);   \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}

#define VECTOR_NORMALIZEA(Dimension, Vec)                                     \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Normalize(temp);\
	DirectX::XMStoreFloat##Dimension##A(Vec, vResult);                        \
}
#define VECTOR_ADDA(Dimension, VecLeft, VecRight, VecResult)                  \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define VECTOR_SUBTRACTA(Dimension, VecLeft, VecRight, VecResult)             \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	v1 = DirectX::XMVectorNegate(v1);                                         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define VECTOR_MULTIPLYA(Dimension, VecLeft, VecRight, VecResult)             \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMultiply(v0, v1);            \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define VECTOR_CROSSA(Dimension, VecLeft, VecRight, VecResult)                \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVector3Cross(v0, v1);              \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define VECTOR_SCALEA(Dimension, Vec, Factor, VecResult)                      \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorScale(temp, Factor);         \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define VECTOR_DOTA(Dimension, VecLeft, VecRight, Result)                     \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Dot(v0, v1);    \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}
#define VECTOR_LENGTHA(Dimension, Vec, Result)                                \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Length(temp);   \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}

#define VECTOR_FUNCTIONS(Dimension)                                           \
inline void operator+=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_ADD(Dimension, this, &right, this)                                 \
}                                                                             \
inline void operator-=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_SUBTRACT(Dimension, this, &right, this)                            \
}                                                                             \
inline void operator*=(const float32_t right)                                 \
{                                                                             \
	VECTOR_SCALE(Dimension, this, right, this)                                \
}                                                                             \
inline void operator*=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_MULTIPLY(Dimension, this, &right, this)                            \
}                                                                             \
inline Vec##Dimension cross(const Vec##Dimension &right)                      \
{                                                                             \
	VECTOR_CROSS(Dimension, this, &right, this)                               \
}                                                                             \
inline float32_t dot(const Vec##Dimension &right) const                       \
{                                                                             \
	float32_t result = 0.0f;                                                  \
	VECTOR_DOT(Dimension, this, &right, result)                               \
	return result;                                                            \
}                                                                             \
inline void normalize()                                                       \
{                                                                             \
	VECTOR_NORMALIZE(Dimension, this)                                         \
}                                                                             \
inline void negate()                                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(this);           \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                \
	DirectX::XMStoreFloat##Dimension(this, vResult);                          \
}                                                                             \
inline float32_t length()                                                     \
{                                                                             \
	float32_t result = std::numeric_limits<float32_t>().max();                \
	VECTOR_LENGTH(Dimension, this, result)                                    \
	return result;                                                            \
}

#define VECTOR_FUNCTIONSA(Dimension)                                          \
inline void operator+=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_ADDA(Dimension, this, &right, this)                                \
}                                                                             \
inline void operator-=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_SUBTRACTA(Dimension, this, &right, this)                           \
}                                                                             \
inline void operator*=(const float32_t factor)                                \
{                                                                             \
	VECTOR_SCALEA(Dimension, this, factor, this)                              \
}                                                                             \
inline void operator*=(const Vec##Dimension &right)                           \
{                                                                             \
	VECTOR_MULTIPLYA(Dimension, this, &right, this)                           \
}                                                                             \
inline Vec##Dimension cross(const Vec##Dimension &right)                      \
{                                                                             \
	VECTOR_CROSSA(Dimension, this, &right, this)                              \
}                                                                             \
inline float32_t dot(const Vec##Dimension &right) const                       \
{                                                                             \
	float32_t result = 0.0f;                                                  \
	VECTOR_DOTA(Dimension, this, &right, result)                              \
	return result;                                                            \
}                                                                             \
inline void normalize()                                                       \
{                                                                             \
	VECTOR_NORMALIZEA(Dimension, this)                                        \
}                                                                             \
inline void negate()                                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(this);        \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                \
	DirectX::XMStoreFloat##Dimension##A(this, vResult);                       \
}                                                                             \
inline float32_t length()                                                     \
{                                                                             \
	float32_t result = std::numeric_limits<float32_t>().max();                \
	VECTOR_LENGTHA(Dimension, this, result)                                   \
	return result;                                                            \
}

#define VECTOR_FUNCTIONS_GLOBAL(Dimension)                                               \
inline Vec##Dimension operator+(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_ADD(Dimension, &left, &right, &result)                                        \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator-(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_SUBTRACT(Dimension, &left, &right, &result)                                   \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator*(const Vec##Dimension &left, const float32_t right)       \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_SCALE(Dimension, &left, right, &result)                                       \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator*(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_MULTIPLY(Dimension, &left, &right, &result)                                   \
	return result;                                                                       \
}                                                                                        \
inline bool8_t operator==(const Vec##Dimension &left, const Vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##Equal(vLeft, vRight);                           \
}                                                                                        \
inline bool8_t operator>=(const Vec##Dimension &left, const Vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##GreaterOrEqual(vLeft, vRight);                  \
}                                                                                        \
inline bool8_t operator>(const Vec##Dimension &left, const Vec##Dimension &right)        \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##Greater(vLeft, vRight);                         \
}                                                                                        \
inline Vec##Dimension cross(const Vec##Dimension &left, const Vec##Dimension &right)     \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_CROSS(Dimension, &left, &right, &result)                                      \
	return result;                                                                       \
}                                                                                        \
inline float32_t dot(const Vec##Dimension &left, const Vec##Dimension &right)            \
{                                                                                        \
	float32_t result;                                                                    \
	VECTOR_DOT(Dimension, &left, &right, result)                                         \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension normalize(const Vec##Dimension &targetVec)                         \
{                                                                                        \
	Vec##Dimension result(targetVec);                                                    \
	VECTOR_NORMALIZE(Dimension, &targetVec)                                              \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension negate(Vec##Dimension &targetVec)                                  \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(&targetVec);                \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                           \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}                                                                                        \
inline float32_t length(const Vec##Dimension &targeVec)                                  \
{                                                                                        \
	float32_t result = std::numeric_limits<float32_t>().max();                           \
	VECTOR_LENGTH(Dimension, &targeVec, result)                                          \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension getMin(const Vec##Dimension &left, const Vec##Dimension &right)    \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(&left);                       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(&right);                      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMin(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension getMax(const Vec##Dimension &left, const Vec##Dimension &right)    \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(&left);                       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(&right);                      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMax(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}

#define VECTOR_FUNCTIONSA_GLOBAL(Dimension)                                              \
inline Vec##Dimension operator+(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_ADDA(Dimension, &left, &right, &result)                                       \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator-(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_SUBTRACTA(Dimension, &left, &right, &result)                                  \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator*(const Vec##Dimension &left, const float32_t right)       \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_SCALEA(Dimension, &left, right, &result)                                      \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension operator*(const Vec##Dimension &left, const Vec##Dimension &right) \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_MULTIPLYA(Dimension, &left, &right, &result)                                  \
	return result;                                                                       \
}                                                                                        \
inline bool8_t operator==(const Vec##Dimension &left, const Vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);                 \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##Equal(vLeft, vRight);                           \
}                                                                                        \
inline bool8_t operator>=(const Vec##Dimension &left, const Vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);                 \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##GreaterOrEqual(vLeft, vRight);                  \
}                                                                                        \
inline bool8_t operator>(const Vec##Dimension &left, const Vec##Dimension &right)        \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);			     \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##Greater(vLeft, vRight);                         \
}                                                                                        \
inline Vec##Dimension cross(const Vec##Dimension &left, const Vec##Dimension &right)     \
{                                                                                        \
	Vec##Dimension result;                                                               \
	VECTOR_CROSSA(Dimension, &left, &right, &result)                                     \
	return result;                                                                       \
}                                                                                        \
inline float32_t dot(const Vec##Dimension &left, const Vec##Dimension &right)            \
{                                                                                        \
	float32_t result = 0.0f;                                                             \
	VECTOR_DOTA(Dimension, &left, &right, result)                                        \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension normalize(const Vec##Dimension &targetVec)                         \
{                                                                                        \
	Vec##Dimension result(targetVec);                                                    \
	VECTOR_NORMALIZEA(Dimension, &result)                                                \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension negate(Vec##Dimension &targetVec)                                  \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(&targetVec);             \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                           \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}                                                                                        \
inline float32_t length(const Vec##Dimension &targeVec)                                  \
{                                                                                        \
	float32_t result = std::numeric_limits<float32_t>().max();                           \
	VECTOR_LENGTHA(Dimension, &targeVec, result)                                         \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension getMin(const Vec##Dimension &left, const Vec##Dimension &right)    \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(&left);                    \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(&right);                   \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMin(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}                                                                                        \
inline Vec##Dimension getMax(const Vec##Dimension &left, const Vec##Dimension &right)    \
{                                                                                        \
	Vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(&left);                    \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(&right);                   \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMax(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}

NAMESPACE_END(Math)
NAMESPACE_END(Gear)

