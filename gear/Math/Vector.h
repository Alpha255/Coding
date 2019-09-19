#pragma once

#include "gear/Public/Util.h"
#include <DirectXMath.h>
#include <DirectXCollision.h>

#define vecNormalize(Dimension, Vec)                                          \
{                                                                             \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec);            \
	DirectX::XMVECTOR result = DirectX::XMVector##Dimension##Normalize(temp); \
	DirectX::XMStoreFloat##Dimension(Vec, result);                            \
}

#define vecAdd(Dimension, VecLeft, VecRight)                       \
{                                                                  \
	DirectX::XMVECTOR temp = DirectX::XMLoadFloat##Dimension(Vec); \
	DirectX::XMVector##Dimension##Normalize(temp);                 \
	DirectX::XMStoreFloat##Dimension(Vec, temp);                   \
}

namespaceStart(gear)

namespaceEnd(end)

