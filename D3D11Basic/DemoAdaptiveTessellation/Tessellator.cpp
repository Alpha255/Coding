#include "Tessellator.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct CB_EdgeFactor
{
	Matrix WVP;

	Vec2 TessEdgeLenScale;
	uint32_t TriCount;
	uint32_t PartitioningMode;
};

void Tessellator::CreateLookupTable()
{	
	uint32_t halfFactor = s_MaxFactor / 2U;
	
	int32_t finalPointPosTable[s_MaxFactor / 2 + 1] = { 0 };
	finalPointPosTable[1] = halfFactor;

	/// Init final_Point_Position_Table
	for (uint32_t i = 2U; i < halfFactor + 1; ++i)
	{
		int32_t level = 0;
		for (;;)
		{
			if (0 == (((i - 2) - ((1UL << level) - 1)) & ((1UL << (level + 1)) - 1)))
			{
				break;
			}
			++level;
		}

		finalPointPosTable[i] = ((halfFactor >> 1) + ((i - 2) - ((1UL << level) - 1))) >> (level + 1);
	}

	/// Init inside_Point_Index_Table / outside_Point_Index_Table
	for (uint32_t i = 0U; i <= halfFactor; ++i)
	{
		for (uint32_t j = 0U; j <= halfFactor; ++j)
		{
			m_InsidePointIndex[i][j][0] = (0 == j ? 0 : (finalPointPosTable[j] < (int32_t)i));

			m_OutsidePointIndex[i][j][0] = (finalPointPosTable[j] < (int32_t)i);
		}

		m_InsidePointIndex[i][halfFactor + 1][0] = 0;
		m_OutsidePointIndex[i][halfFactor + 1][0] = 0;

		for (uint32_t k = 0; k <= halfFactor + 1; ++k)
		{
			m_InsidePointIndex[i][k][1] = (0 == k ? 0 : (m_InsidePointIndex[i][k - 1][0] + m_InsidePointIndex[i][k - 1][1]));

			m_OutsidePointIndex[i][k][1] = (0 == k ? 0 : (m_OutsidePointIndex[i][k - 1][0] + m_OutsidePointIndex[i][k - 1][1]));

			if (m_InsidePointIndex[i][k][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][k][1]][2] = k;
			}

			if (m_OutsidePointIndex[i][k][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][k][1]][2] = k;
			}
		}

		for (uint32_t m = halfFactor; m >= 0; --m)
		{
			if (m_InsidePointIndex[i][m][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][halfFactor + 1][1] - m_InsidePointIndex[i][m + 1][1]][3] = m;
			}

			if (m_OutsidePointIndex[i][m][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][halfFactor + 1][1] - m_OutsidePointIndex[i][m + 1][1]][3] = m;
			}
		}
	}
}

void Tessellator::CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf)
{
	assert(g_Renderer && pVertexBuf);

	g_Renderer->CreateComputeShader(m_Res.EdgeFactor.Reference(), "Tessellator_EdgeFactor.hlsl", "Func_EdgeFactor");
	g_Renderer->CreateComputeShader(m_Res.ScatterVertexTriID_IndexID.Reference(), "Tessellator_ScatterID.hlsl", "Func_ScatterVertexTriID_IndexID");
	g_Renderer->CreateComputeShader(m_Res.ScatterIndexTriID_IndexID.Reference(), "Tessellator_ScatterID.hlsl", "Func_ScatterIndexTriID_IndexID");

	g_Renderer->CreateComputeShader(m_Res.NumVI.Reference(), "Tessellator_NumVI", "Func_NumVI");
	g_Renderer->CreateComputeShader(m_Res.TessVertex.Reference(), "Tessellator_TessVertex", "Func_TessVertex");
	g_Renderer->CreateComputeShader(m_Res.TessIndex.Reference(), "Tessellator_TessIndex", "Func_TessIndex");

	int32_t lut[sizeof(m_InsidePointIndex) + sizeof(m_OutsidePointIndex) / sizeof(int32_t)] = { 0 };
	memcpy(lut, m_InsidePointIndex, sizeof(m_InsidePointIndex));
	memcpy(&lut[sizeof(m_InsidePointIndex) / sizeof(int32_t)], m_OutsidePointIndex, sizeof(m_OutsidePointIndex));
	g_Renderer->CreateConstantBuffer(m_Res.CB_LookupTable.Reference(), sizeof(lut), D3D11_USAGE_IMMUTABLE, lut);

	g_Renderer->CreateConstantBuffer(m_Res.CB_EdgeFactor.Reference(), sizeof(CB_EdgeFactor), D3D11_USAGE_DEFAULT, nullptr);

	g_Renderer->CreateConstantBuffer(m_Res.CB_Tess.Reference(), sizeof(uint32_t) * 4, D3D11_USAGE_DEFAULT, nullptr);

	g_Renderer->CreateConstantBuffer(m_Res.CB_ReadBackBuf.Reference(), sizeof(uint32_t) * 2, D3D11_USAGE_STAGING, nullptr);

	g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Src.Reference(), pVertexBuf, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount);

	Ref<ID3D11Buffer> edgeFactor;
	g_Renderer->CreateUnorderedAccessBuffer(edgeFactor.Reference(), sizeof(float) * vertexCount / 3U * 4U, 
		D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * 4);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_EdgeFactor.Reference(), edgeFactor.Ptr(),
		DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_EdgeFactor.Reference(), edgeFactor.Ptr(),
		DXGI_FORMAT_UNKNOWN, D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);
}

void Tessellator::ExeComputeShader(
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
	uint32_t patchZ)
{
	g_Renderer->SetComputeShader(pTargetCS);
	for (uint32_t i = 0U; i < srvCount; ++i)
	{
		g_Renderer->SetShaderResource(ppSRVs[i], i, 1U, D3DGraphic::eComputeShader);
	}
	g_Renderer->SetUnorderedAccessView(pUAV);

	if (pCBUpdate)
	{
		g_Renderer->UpdateBuffer(pCBUpdate, pBufData, cbufSize);
	}

	if (pCBConstant && pCBUpdate)
	{
		g_Renderer->SetConstantBuffer(pCBConstant, 0U, D3DGraphic::eComputeShader);
		g_Renderer->SetConstantBuffer(pCBUpdate, 1U, D3DGraphic::eComputeShader);
	}

	if (pCBConstant && !pCBUpdate)
	{
		g_Renderer->SetConstantBuffer(pCBConstant, 0U, D3DGraphic::eComputeShader);
	}

	if (!pCBConstant && pCBUpdate)
	{
		g_Renderer->SetConstantBuffer(pCBUpdate, 0U, D3DGraphic::eComputeShader);
	}

	g_Renderer->ExeCommandlist(patchX, patchY, patchZ);

	g_Renderer->SetUnorderedAccessView(nullptr);
	for (uint32_t i = 0U; i < srvCount; ++i)
	{
		g_Renderer->SetShaderResource(nullptr, i, 1U, D3DGraphic::eComputeShader);
	}
	g_Renderer->SetConstantBuffer(nullptr, 1U, D3DGraphic::eComputeShader);
}

uint32_t Tessellator::DoTessellationByEdge(const Camera &cam)
{
	uint32_t tessedVertexCount = 0U, tessedIndexCount = 0U;

	/// Update per-edge tessellation factors
	{
		CB_EdgeFactor cbuf;
		cbuf.WVP = cam.GetWorldMatrix() * cam.GetViewMatrix() * cam.GetProjMatrix();
		cbuf.PartitioningMode = m_ptMode;
		cbuf.TessEdgeLenScale = m_EdgeLenScale;
		cbuf.TriCount = m_SrcVertexCount / 3U;

		ID3D11ShaderResourceView *ppSRV[1] = { m_Res.SRV_VB_Src.Ptr() };
		ExeComputeShader(
			m_Res.EdgeFactor.Ptr(),
			1U,
			ppSRV,
			nullptr,
			m_Res.CB_EdgeFactor.Ptr(),
			sizeof(CB_EdgeFactor),
			&cbuf,
			m_Res.UAV_EdgeFactor.Ptr(),
			(uint32_t)ceilf(m_SrcVertexCount / 3.0f / 128.0f),
			1U,
			1U);
	}

	/// How many vertices/indices are needed for the tessellated mesh?
	{
		uint32_t cbuf[4] = { m_SrcVertexCount / 3U, 0U, 0U, 0U };
		ID3D11ShaderResourceView *ppSRV[1] = { m_Res.SRV_EdgeFactor.Ptr() };
		ExeComputeShader(
			m_Res.NumVI.Ptr(),
			1U,
			ppSRV,
			m_Res.CB_LookupTable.Ptr(),
			m_Res.CB_Tess.Ptr(),
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Scanner.GetScanUAV0(),
			(uint32_t)ceilf(m_SrcVertexCount / 3.0f / 128.0f),
			1U,
			1U);
		m_Scanner.Scan(m_SrcVertexCount / 3U);

		/// Read back the number of vertices/indices for tessellation output
		uint32_t copyBuf[2] = { 0U };
		::RECT srcRect =
		{
			sizeof(uint32_t) * 2U * m_SrcVertexCount / 3U - sizeof(uint32_t) * 2U,
			0U,
			sizeof(uint32_t) * 2U * m_SrcVertexCount / 3U,
			1U
		};
		g_Renderer->CopyBuffer(m_Res.CB_ReadBackBuf.Ptr(), m_Scanner.GetScanBuf(), copyBuf, sizeof(uint32_t) * 2U, srcRect);
		tessedVertexCount = copyBuf[0];
		tessedIndexCount = copyBuf[1];
	}

	/// Generate buffers for scattering TriID and IndexID for both vertex data and index data,
	/// also generate buffers for output tessellated vertex data and index data
	static Ref<ID3D11Buffer> NullBuf;
	static Ref<ID3D11ShaderResourceView> NullSRV;
	static Ref<ID3D11UnorderedAccessView> NullUAV;

	{
		if (!m_Res.VB_Scatter.Valid() || m_CachedTessedVertexCount < tessedVertexCount)
		{
			m_Res.VB_Scatter = NullBuf;
			m_Res.SRV_VB_Scatter = NullSRV;
			m_Res.UAV_VB_Scatter = NullUAV;

			m_Res.VB_Tessed = NullBuf;
			m_Res.SRV_VB_Tessed = NullSRV;
			m_Res.UAV_VB_Tessed = NullUAV;

			g_Renderer->CreateUnorderedAccessBuffer(m_Res.VB_Scatter.Reference(), sizeof(uint32_t) * 2U * tessedVertexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2U);
			g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Scatter.Reference(), m_Res.VB_Scatter.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_SRV_DIMENSION_BUFFER, 0U, tessedVertexCount);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_VB_Scatter.Reference(), m_Res.VB_Scatter.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_UAV_DIMENSION_BUFFER, 0U, tessedVertexCount);

			/// Create the output tessellated vertices buffer
			g_Renderer->CreateUnorderedAccessBuffer(m_Res.VB_Tessed.Reference(), sizeof(float) * 3U * tessedVertexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * 3U);
			g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Scatter.Reference(), m_Res.VB_Tessed.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_SRV_DIMENSION_BUFFER, 0U, tessedVertexCount);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_VB_Scatter.Reference(), m_Res.VB_Tessed.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_UAV_DIMENSION_BUFFER, 0U, tessedVertexCount);

			m_CachedTessedVertexCount = tessedVertexCount;
		}
	}

	{

	}

	/// Scatter TriID, IndexID

	/// Tessellate vertex

	/// Tessellate indices

	return tessedIndexCount;
}
