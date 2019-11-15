#pragma once

#include "Vector.h"

namespaceStart(gear)
namespaceStart(math)

constexpr float32_t g_pi = DirectX::XM_PI;
constexpr float32_t g_2pi = DirectX::XM_2PI;
constexpr float32_t g_1div_pi = DirectX::XM_1DIVPI;
constexpr float32_t g_1div_2pi = DirectX::XM_1DIV2PI;
constexpr float32_t g_pi_div2 = DirectX::XM_PIDIV2;
constexpr float32_t g_pi_div4 = DirectX::XM_PIDIV4;
constexpr float32_t g_epsilon = 1e-6f;

template<typename T> inline T clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}

namespaceEnd(math)
namespaceEnd(gear)
