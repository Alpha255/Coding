#pragma once

#include "D3DGraphic.h"

class Tessellator
{
public:
	enum ePartitioningMode
	{
		eInteger,
		ePow2,
		eFractionalOdd,
		eFractionalEven,
		ePartitioningModeCount
	};

	Tessellator() = default;
	~Tessellator() = default;

	inline ePartitioningMode GetPartitioningMode() const
	{
		return m_ptMode;
	}
	inline void SetPartitioningMode(ePartitioningMode mode)
	{
		m_ptMode = mode;
	}

	inline void Resize(uint32_t width, uint32_t height)
	{
		const float adaptiveScaleInPixels = 15.0f;
		m_EdgeLenScale.x = width * 0.5f / adaptiveScaleInPixels;
		m_EdgeLenScale.y = height * 0.5f / adaptiveScaleInPixels;
	}

	inline void Init(uint32_t vertexCount, ID3D11Buffer *pVertexBuf)
	{
		CreateLookupTable();
		CreateResource(vertexCount, pVertexBuf);
	}
protected:
	void CreateLookupTable();
	void CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf);
private:
	ePartitioningMode m_ptMode = eFractionalEven;
	Vec2 m_EdgeLenScale = Vec2(0.0f, 0.0f);
	static const uint32_t s_MaxFactor = 16U;

	int32_t m_InsidePointIndex[s_MaxFactor / 2 + 1][s_MaxFactor / 2 + 2][4] = { 0 };
	int32_t m_OutsidePointIndex[s_MaxFactor / 2 + 1][s_MaxFactor / 2 + 2][4] = { 0 };

	struct D3DResource
	{
		Ref<ID3D11ComputeShader> EdgeFactor;
		Ref<ID3D11ComputeShader> NumVI;
		Ref<ID3D11ComputeShader> ScatterVertexTriID_IndexID;
		Ref<ID3D11ComputeShader> ScatterIndexTriID_IndexID;
		Ref<ID3D11ComputeShader> TessVertex;
		Ref<ID3D11ComputeShader> TessIndex;

		Ref<ID3D11Buffer> CB_EdgeFactor;
		Ref<ID3D11Buffer> CB_LookupTable;
		Ref<ID3D11Buffer> CB_Tess;
		Ref<ID3D11Buffer> CB_ReadBackBuf;
	}m_Res;
};
