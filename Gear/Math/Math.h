#pragma once

#include "Gear/Math/AABB.h"
#include "Gear/Math/Color.h"
#include "Gear/Math/Euler.h"
#include "Gear/Math/Frustum.h"
#include "Gear/Math/Matrix.h"
#include "Gear/Math/OBB.h"
#include "Gear/Math/Plane.h"
#include "Gear/Math/Quaternions.h"
#include "Gear/Math/Rect.h"
#include "Gear/Math/Sphere.h"
#include "Gear/Math/Transform.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Math)

static constexpr float32_t PI = DirectX::XM_PI;
static constexpr float32_t PI_2 = DirectX::XM_2PI;
static constexpr float32_t PI_Inv = DirectX::XM_1DIVPI;
static constexpr float32_t PI_2_Inv = DirectX::XM_1DIV2PI;
static constexpr float32_t PI_Div2 = DirectX::XM_PIDIV2;
static constexpr float32_t PI_Div4 = DirectX::XM_PIDIV4;
static constexpr float32_t Epsilon = std::numeric_limits<float32_t>().epsilon();

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

NAMESPACE_END(Math)
NAMESPACE_END(Gear)