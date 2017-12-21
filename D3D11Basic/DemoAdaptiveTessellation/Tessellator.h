#pragma once

#include "Scanner.h"

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
		m_SrcVertexCount = vertexCount;

		CreateLookupTable();
		CreateResource(vertexCount, pVertexBuf);

		m_Scanner.Init(vertexCount);
	}

	inline uint32_t GetTessedVertexCount()
	{
		return m_CachedTessedVertexCount;
	}

	inline uint32_t GetTessedIndexCount()
	{
		return m_CachedTessedIndexCount;
	}

	inline ID3D11ShaderResourceView *GetSourceVertexBufferSRV()
	{
		return m_Res.SRV_VB_Src.Ptr();
	}

	inline ID3D11ShaderResourceView *GetTessedVertexBufferSRV()
	{
		return m_Res.SRV_VB_Tessed.Ptr();
	}

	inline ID3D11Buffer *GetTessedIndexBuffer()
	{
		return m_Res.IB_Tessed.Ptr();
	}

	bool DoTessellationByEdge(const class Camera &cam);
protected:
	void CreateLookupTable();
	void CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf);

	void ExeComputeShader(
		ID3D11ComputeShader *pTargetCS, 
		uint32_t srvCount, 
		ID3D11ShaderResourceView **ppSRVs, 
		ID3D11Buffer *pCBConstant, 
		ID3D11Buffer *pCBUpdate, 
		uint32_t cbufSize, 
		const void *pBufData,
		ID3D11UnorderedAccessView *pUAV,
		uint32_t patchX, 
		uint32_t patchY, 
		uint32_t patchZ);
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

		Ref<ID3D11Buffer> VB_Scatter;
		Ref<ID3D11Buffer> IB_Scatter;

		Ref<ID3D11Buffer> VB_Tessed;
		Ref<ID3D11Buffer> IB_Tessed;

		Ref<ID3D11ShaderResourceView> SRV_VB_Src;
		Ref<ID3D11ShaderResourceView> SRV_VB_Tessed;
		Ref<ID3D11ShaderResourceView> SRV_EdgeFactor;

		Ref<ID3D11ShaderResourceView> SRV_VB_Scatter;
		Ref<ID3D11ShaderResourceView> SRV_IB_Scatter;

		Ref<ID3D11UnorderedAccessView> UAV_EdgeFactor;

		Ref<ID3D11UnorderedAccessView> UAV_VB_Scatter;
		Ref<ID3D11UnorderedAccessView> UAV_IB_Scatter;

		Ref<ID3D11UnorderedAccessView> UAV_VB_Tessed;
		Ref<ID3D11UnorderedAccessView> UAV_IB_Tessed;
	}m_Res;

	Scanner m_Scanner;

	uint32_t m_SrcVertexCount = 0U;
	uint32_t m_CachedTessedVertexCount = 0U;
	uint32_t m_CachedTessedIndexCount = 0U;
};
