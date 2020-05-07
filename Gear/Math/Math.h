#pragma once

#include "AABB.h"
#include "Color.h"
#include "Euler.h"
#include "Frustum.h"
#include "Matrix.h"
#include "OBB.h"
#include "Plane.h"
#include "Quaternions.h"
#include "Rect.h"
#include "Sphere.h"
#include "Transform.h"

namespaceStart(Gear)
namespaceStart(Math)

static constexpr float32_t PI = DirectX::XM_PI;
static constexpr float32_t PI_2 = DirectX::XM_2PI;
static constexpr float32_t PI_Inv = DirectX::XM_1DIVPI;
static constexpr float32_t PI_2_Inv = DirectX::XM_1DIV2PI;
static constexpr float32_t PI_Div2 = DirectX::XM_PIDIV2;
static constexpr float32_t PI_Div4 = DirectX::XM_PIDIV4;
static constexpr float32_t Epsilon = 1e-6f;

template<typename T> inline T clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}

inline float32_t radians(float32_t degree)
{
	return DirectX::XMConvertToRadians(degree);
}

inline float32_t degree(float32_t radians)
{
	return DirectX::XMConvertToDegrees(radians);
}

namespaceEnd(Math)
namespaceEnd(Gear)