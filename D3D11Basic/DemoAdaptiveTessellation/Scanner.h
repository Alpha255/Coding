#pragma once

#include "D3DGraphic.h"

class Scanner
{
public:
	Scanner() = default;
	~Scanner() = default;

	void Init(uint32_t vertexCount);
	void Scan(uint32_t numToScan);

	inline ID3D11UnorderedAccessView *GetScanUAV0()
	{
		assert(m_Res.UAV_Scan0.Valid());

		return m_Res.UAV_Scan0.Ptr();
	}

	inline ID3D11Buffer *GetScanBuf()
	{
		assert(m_Res.CB_Scan.Valid());

		return m_Res.CB_Scan.Ptr();
	}

	inline ID3D11ShaderResourceView *GetScanSRV0()
	{
		assert(m_Res.SRV_Scan0.Valid());

		return m_Res.SRV_Scan0.Ptr();
	}
protected:
private:
	struct D3DResource
	{
		Ref<ID3D11ComputeShader> ScanInBucket;
		Ref<ID3D11ComputeShader> ScanBucketResult;
		Ref<ID3D11ComputeShader> ScanAddBucketResult;

		Ref<ID3D11Buffer> CB_Scan;

		Ref<ID3D11ShaderResourceView> SRV_Aux;
		Ref<ID3D11ShaderResourceView> SRV_Scan0;
		Ref<ID3D11ShaderResourceView> SRV_Scan1;

		Ref<ID3D11UnorderedAccessView> UAV_Aux;
		Ref<ID3D11UnorderedAccessView> UAV_Scan0;
		Ref<ID3D11UnorderedAccessView> UAV_Scan1;
	}m_Res;
};
