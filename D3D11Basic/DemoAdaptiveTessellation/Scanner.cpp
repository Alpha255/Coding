#include "Scanner.h"

extern D3DGraphic* g_Renderer;

struct ConstantsBuf
{
	uint32_t Params[4] = { 0 };
};

void Scanner::Init(uint32_t vertexCount)
{
	assert(g_Renderer);

	static char *s_ShaderName = "Scanner.hlsl";
	g_Renderer->CreateComputeShader(m_Res.ScanInBucket.Reference(), s_ShaderName, "Func_ScanInBucket");
	g_Renderer->CreateComputeShader(m_Res.ScanBucketResult.Reference(), s_ShaderName, "Func_ScanBucketResult");
	g_Renderer->CreateComputeShader(m_Res.ScanAddBucketResult.Reference(), s_ShaderName, "Func_ScanAddBucketResult");

	g_Renderer->CreateConstantBuffer(m_Res.CB_Scan.Reference(), sizeof(ConstantsBuf), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	Ref<ID3D11Buffer> auxBuf;
	g_Renderer->CreateUnorderedAccessBuffer(auxBuf.Reference(), sizeof(uint32_t) * 2U * 128U, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);

	g_Renderer->CreateShaderResourceView(m_Res.SRV_Aux.Reference(), auxBuf.Ptr(), DXGI_FORMAT_UNKNOWN, 
		D3D11_SRV_DIMENSION_BUFFER, 0U, 128U);

	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Aux.Reference(), auxBuf.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, 128U);

	Ref<ID3D11Buffer> scan0;
	g_Renderer->CreateUnorderedAccessBuffer(scan0.Reference(), sizeof(uint32_t) * 2 * vertexCount / 3, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_Scan0.Reference(), scan0.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Scan0.Reference(), scan0.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);

	Ref<ID3D11Buffer> scan1;
	g_Renderer->CreateUnorderedAccessBuffer(scan1.Reference(), sizeof(uint32_t) * 2 * vertexCount / 3, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_Scan1.Reference(), scan1.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Scan1.Reference(), scan1.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);
}

void Scanner::Scan(uint32_t numToScan)
{
	/// First pass, scan in each bucket
	{
		g_Renderer->SetComputeShader(m_Res.ScanInBucket.Ptr());
		g_Renderer->SetShaderResource(m_Res.SRV_Scan0.Ptr(), 0U, 1U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Scan1.Ptr());
		g_Renderer->ExeCommandlist((uint32_t)ceilf(numToScan / 128.0f), 1U, 1U);
		g_Renderer->SetUnorderedAccessView(nullptr);
	}

	/// Second pass, record and scan the sum of each bucket
	{
		g_Renderer->SetComputeShader(m_Res.ScanBucketResult.Ptr());
		g_Renderer->SetShaderResource(m_Res.SRV_Scan1.Ptr(), 0U, 1U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Aux.Ptr());
		g_Renderer->ExeCommandlist(1U, 1U, 1U);
		g_Renderer->SetUnorderedAccessView(nullptr);
	}

	/// Last pass, add the buckets scanned result to each bucket to get the final result
	{
		g_Renderer->SetComputeShader(m_Res.ScanAddBucketResult.Ptr());
		g_Renderer->SetShaderResource(m_Res.SRV_Scan1.Ptr(), 0U, 1U, D3DGraphic::eComputeShader);
		g_Renderer->SetShaderResource(m_Res.SRV_Aux.Ptr(), 1U, 1U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Scan0.Ptr());
		g_Renderer->ExeCommandlist((uint32_t)ceilf(numToScan / 128.0f), 1U, 1U);
	}

	/// Restore state
	g_Renderer->SetUnorderedAccessView(nullptr);
	g_Renderer->SetShaderResource(nullptr, 0U, 1U, D3DGraphic::eComputeShader);
	g_Renderer->SetShaderResource(nullptr, 1U, 1U, D3DGraphic::eComputeShader);
}
