#pragma once

#include "Gear/Public/Definitions.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

///#define UsingSSE

namespaceStart(gear)
namespaceStart(math)

#define vecNormalize(Dimension, Vec)                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Normalize(temp);\
	DirectX::XMStoreFloat##Dimension(Vec, vResult);                           \
}
#define vecAdd(Dimension, VecLeft, VecRight, VecResult)                       \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define vecSub(Dimension, VecLeft, VecRight, VecResult)                       \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	v1 = DirectX::XMVectorNegate(v1);                                         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define vecMultiply(Dimension, VecLeft, VecRight, VecResult)                  \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMultiply(v0, v1);            \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define vecCross(Dimension, VecLeft, VecRight, VecResult)                     \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector3Cross(v0, v1);              \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define vecScale(Dimension, Vec, Factor, VecResult)                           \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVectorScale(temp, Factor);         \
	DirectX::XMStoreFloat##Dimension(VecResult, vResult);                     \
}
#define vecDot(Dimension, VecLeft, VecRight, Result)                          \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(VecLeft);          \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(VecRight);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Dot(v0, v1);    \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}
#define vecLength(Dimension, Vec, Result)                                     \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Length(temp);   \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}

#define vecNormalizeA(Dimension, Vec)                                         \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Normalize(temp);\
	DirectX::XMStoreFloat##Dimension##A(Vec, vResult);                        \
}
#define vecAddA(Dimension, VecLeft, VecRight, VecResult)                      \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define vecSubA(Dimension, VecLeft, VecRight, VecResult)                      \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	v1 = DirectX::XMVectorNegate(v1);                                         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorAdd(v0, v1);                 \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define vecMultiplyA(Dimension, VecLeft, VecRight, VecResult)                 \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMultiply(v0, v1);            \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define vecCrossA(Dimension, VecLeft, VecRight, VecResult)                    \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVector3Cross(v0, v1);              \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define vecScaleA(Dimension, Vec, Factor, VecResult)                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVectorScale(temp, Factor);         \
	DirectX::XMStoreFloat##Dimension##A(VecResult, vResult);                  \
}
#define vecDotA(Dimension, VecLeft, VecRight, Result)                         \
{                                                                             \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(VecLeft);       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(VecRight);      \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Dot(v0, v1);    \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}
#define vecLengthA(Dimension, Vec, Result)                                    \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(Vec);         \
	DirectX::XMVECTOR vResult = DirectX::XMVector##Dimension##Length(temp);   \
	Result = DirectX::XMVectorGetX(vResult);                                  \
}

#define vecMemberFuncs(Dimension)                                             \
inline void operator+=(const vec##Dimension &right)                           \
{                                                                             \
	vecAdd(Dimension, this, &right, this)                                     \
}                                                                             \
inline void operator-=(const vec##Dimension &right)                           \
{                                                                             \
	vecSub(Dimension, this, &right, this)                                     \
}                                                                             \
inline void operator*=(const float32_t right)                                 \
{                                                                             \
	vecScale(Dimension, this, right, this)                                    \
}                                                                             \
inline void operator*=(const vec##Dimension &right)                           \
{                                                                             \
	vecMultiply(Dimension, this, &right, this)                                \
}                                                                             \
inline vec##Dimension cross(const vec##Dimension &right)                      \
{                                                                             \
	vecCross(Dimension, this, &right, this)                                   \
}                                                                             \
inline float32_t dot(const vec##Dimension &right) const                       \
{                                                                             \
	float32_t result = 0.0f;                                                  \
	vecDot(Dimension, this, &right, result)                                   \
	return result;                                                            \
}                                                                             \
inline void normalize()                                                       \
{                                                                             \
	vecNormalize(Dimension, this)                                             \
}                                                                             \
inline void negate()                                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(this);           \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                \
	DirectX::XMStoreFloat##Dimension(this, vResult);                          \
}                                                                             \
inline float32_t length()                                                     \
{                                                                             \
	float32_t result = FLT_MAX;                                               \
	vecLength(Dimension, this, result)                                        \
	return result;                                                            \
}

#define vecMemberFuncsA(Dimension)                                            \
inline void operator+=(const vec##Dimension &right)                           \
{                                                                             \
	vecAddA(Dimension, this, &right, this)                                    \
}                                                                             \
inline void operator-=(const vec##Dimension &right)                           \
{                                                                             \
	vecSubA(Dimension, this, &right, this)                                    \
}                                                                             \
inline void operator*=(const float32_t factor)                                \
{                                                                             \
	vecScaleA(Dimension, this, factor, this)                                  \
}                                                                             \
inline void operator*=(const vec##Dimension &right)                           \
{                                                                             \
	vecMultiplyA(Dimension, this, &right, this)                               \
}                                                                             \
inline vec##Dimension cross(const vec##Dimension &right)                      \
{                                                                             \
	vecCrossA(Dimension, this, &right, this)                                  \
}                                                                             \
inline float32_t dot(const vec##Dimension &right) const                       \
{                                                                             \
	float32_t result = 0.0f;                                                  \
	vecDotA(Dimension, this, &right, result)                                  \
	return result;                                                            \
}                                                                             \
inline void normalize()                                                       \
{                                                                             \
	vecNormalizeA(Dimension, this)                                            \
}                                                                             \
inline void negate()                                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(this);        \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                \
	DirectX::XMStoreFloat##Dimension##A(this, vResult);                       \
}                                                                             \
inline float32_t length()                                                     \
{                                                                             \
	float32_t result = FLT_MAX;                                               \
	vecLengthA(Dimension, this, result)                                       \
	return result;                                                            \
}

#define vecPublicFuncs(Dimension)                                                        \
inline vec##Dimension operator+(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecAdd(Dimension, &left, &right, &result)                                            \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator-(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecSub(Dimension, &left, &right, &result)                                            \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator*(const vec##Dimension &left, const float32_t right)       \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecScale(Dimension, &left, right, &result)                                           \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator*(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecMultiply(Dimension, &left, &right, &result)                                       \
	return result;                                                                       \
}                                                                                        \
inline bool8_t operator==(const vec##Dimension &left, const vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##Equal(vLeft, vRight);                           \
}                                                                                        \
inline bool8_t operator>=(const vec##Dimension &left, const vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##GreaterOrEqual(vLeft, vRight);                  \
}                                                                                        \
inline bool8_t operator>(const vec##Dimension &left, const vec##Dimension &right)        \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension(&left);                    \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension(&right);                  \
	return DirectX::XMVector##Dimension##Greater(vLeft, vRight);                         \
}                                                                                        \
inline vec##Dimension cross(const vec##Dimension &left, const vec##Dimension &right)     \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecCross(Dimension, &left, &right, &result)                                          \
	return result;                                                                       \
}                                                                                        \
inline float32_t dot(const vec##Dimension &left, const vec##Dimension &right)            \
{                                                                                        \
	float32_t result;                                                                    \
	vecDot(Dimension, &left, &right, result)                                             \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension normalize(const vec##Dimension &targetVec)                         \
{                                                                                        \
	vec##Dimension result(targetVec);                                                    \
	vecNormalize(Dimension, &targetVec)                                                  \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension negate(vec##Dimension &targetVec)                                  \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(&targetVec);                \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                           \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}                                                                                        \
inline float32_t length(const vec##Dimension &targeVec)                                  \
{                                                                                        \
	float32_t result = FLT_MAX;                                                          \
	vecLength(Dimension, &targeVec, result)                                              \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension getMin(const vec##Dimension &left, const vec##Dimension &right)    \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(&left);                       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(&right);                      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMin(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension getMax(const vec##Dimension &left, const vec##Dimension &right)    \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension(&left);                       \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension(&right);                      \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMax(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension(&result, vResult);                                  \
	return result;                                                                       \
}

#define vecPublicFuncsA(Dimension)                                                       \
inline vec##Dimension operator+(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecAddA(Dimension, &left, &right, &result)                                           \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator-(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecSubA(Dimension, &left, &right, &result)                                           \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator*(const vec##Dimension &left, const float32_t right)       \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecScaleA(Dimension, &left, right, &result)                                          \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension operator*(const vec##Dimension &left, const vec##Dimension &right) \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecMultiplyA(Dimension, &left, &right, &result)                                      \
	return result;                                                                       \
}                                                                                        \
inline bool8_t operator==(const vec##Dimension &left, const vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);                 \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##Equal(vLeft, vRight);                           \
}                                                                                        \
inline bool8_t operator>=(const vec##Dimension &left, const vec##Dimension &right)       \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);                 \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##GreaterOrEqual(vLeft, vRight);                  \
}                                                                                        \
inline bool8_t operator>(const vec##Dimension &left, const vec##Dimension &right)        \
{                                                                                        \
	DirectX::XMVECTOR vLeft = DirectX::XMLoadFloat##Dimension##A(&left);			     \
	DirectX::XMVECTOR vRight = DirectX::XMLoadFloat##Dimension##A(&right);               \
	return DirectX::XMVector##Dimension##Greater(vLeft, vRight);                         \
}                                                                                        \
inline vec##Dimension cross(const vec##Dimension &left, const vec##Dimension &right)     \
{                                                                                        \
	vec##Dimension result;                                                               \
	vecCrossA(Dimension, &left, &right, &result)                                         \
	return result;                                                                       \
}                                                                                        \
inline float32_t dot(const vec##Dimension &left, const vec##Dimension &right)            \
{                                                                                        \
	float32_t result = 0.0f;                                                             \
	vecDotA(Dimension, &left, &right, result)                                            \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension normalize(const vec##Dimension &targetVec)                         \
{                                                                                        \
	vec##Dimension result(targetVec);                                                    \
	vecNormalizeA(Dimension, &result)                                                    \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension negate(vec##Dimension &targetVec)                                  \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension##A(&targetVec);             \
	DirectX::XMVECTOR vResult = DirectX::XMVectorNegate(temp);                           \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}                                                                                        \
inline float32_t length(const vec##Dimension &targeVec)                                  \
{                                                                                        \
	float32_t result = FLT_MAX;                                                          \
	vecLengthA(Dimension, &targeVec, result)                                             \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension getMin(const vec##Dimension &left, const vec##Dimension &right)    \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(&left);                    \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(&right);                   \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMin(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}                                                                                        \
inline vec##Dimension getMax(const vec##Dimension &left, const vec##Dimension &right)    \
{                                                                                        \
	vec##Dimension result;                                                               \
	DirectX::XMVECTOR v0 = DirectX::XMLoadFloat##Dimension##A(&left);                    \
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat##Dimension##A(&right);                   \
	DirectX::XMVECTOR vResult = DirectX::XMVectorMax(v0, v1);                            \
	DirectX::XMStoreFloat##Dimension##A(&result, vResult);                               \
	return result;                                                                       \
}

namespaceEnd(math)
namespaceEnd(gear)

