#include "D3DMath.h"

void Vec4::TransformCoord(const Matrix &matrix)
{
	DirectX::XMVECTOR srcVec = DirectX::XMLoadFloat4(this);

	DirectX::XMVECTOR result = DirectX::XMVector3Transform(srcVec, *(DirectX::XMMATRIX *)&matrix);

	DirectX::XMStoreFloat4(this, result);
}

void Vec4::TransformNormal(const Matrix &matrix)
{
	DirectX::XMVECTOR srcVec = DirectX::XMLoadFloat4(this);

	DirectX::XMVECTOR result = DirectX::XMVector3TransformNormal(srcVec, *(DirectX::XMMATRIX *)&matrix);

	DirectX::XMStoreFloat4(this, result);
}