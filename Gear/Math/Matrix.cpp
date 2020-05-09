#include "Matrix.h"

namespaceStart(Gear)
namespaceStart(Math)

#if !defined(UsingSSE)
void Matrix::gaussJordanInverse()
{
	/// Gauss-Jordan method
	Matrix copy(*this);
	int32_t i, j, k;

	/// Forward elimination
	for (i = 0; i < 3; ++i)
	{
		int32_t pivot = i;
		float32_t pivotSize = copy.m[i][i];

		pivotSize = pivotSize < 0.0f ? -pivotSize : pivotSize;

		for (j = i + 1; j < 4; ++j)
		{
			float32_t temp = copy.m[j][i];
			temp = temp < 0.0f ? -temp : temp;

			if (temp > pivotSize)
			{
				pivot = j;
				pivotSize = temp;
			}
		}

		/// singular Matrix
		assert(pivotSize != 0.0f);

		if (pivot != i)
		{
			for (j = 0; j < 4; ++j)
			{
				std::swap(copy.m[i][j], copy.m[pivot][j]);
				std::swap(m[i][j], m[pivot][j]);
			}
		}

		for (j = i + 1; j < 4; ++j)
		{
			float32_t factor = copy.m[j][i] / copy.m[i][i];
			for (k = 0; k < 4; ++k)
			{
				copy.m[j][k] -= factor * copy.m[i][k];
				m[j][k] -= factor * m[i][k];
			}
		}
	}

	/// Backward substitution
	for (i = 3; i >= 0; --i)
	{
		float32_t factor = copy.m[i][i];

		/// singular Matrix
		assert(factor != 0.0f);

		for (j = 0; j < 4; ++j)
		{
			copy.m[i][j] /= factor;
			m[i][j] /= factor;
		}

		for (j = 0; j < i; ++j)
		{
			factor = copy.m[j][i];
			for (k = 0; k < 4; ++k)
			{
				copy.m[j][k] -= factor * copy.m[i][k];
				m[j][k] -= factor * m[i][k];
			}
		}
	}
}
#endif

Vec2 Vec2::transformCoord(const Vec2 &src, const Matrix &mat)
{
	DirectX::XMVECTOR vVec = DirectX::XMLoadFloat2A(&src);
	DirectX::XMMATRIX vMat = DirectX::XMLoadFloat4x4A(&mat);
	DirectX::XMVECTOR vResult = DirectX::XMVector2TransformCoord(vVec, vMat);

	Vec2 result;
	DirectX::XMStoreFloat2A(&result, vResult);

	return result;
}

Vec3 Vec3::transformCoord(const Vec3 &src, const Matrix &mat)
{
	DirectX::XMVECTOR vVec = DirectX::XMLoadFloat3A(&src);
	DirectX::XMMATRIX vMat = DirectX::XMLoadFloat4x4A(&mat);
	DirectX::XMVECTOR vResult = DirectX::XMVector3TransformCoord(vVec, vMat);

	Vec3 result;
	DirectX::XMStoreFloat3A(&result, vResult);

	return result;
}

namespaceEnd(Math)
namespaceEnd(Gear)