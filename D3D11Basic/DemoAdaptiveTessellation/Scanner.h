#pragma once

#include "D3DGraphic.h"

class Scanner
{
public:
	Scanner() = default;
	~Scanner() = default;

	void Init(uint32_t vertexCount);
	void Scan(uint32_t numToScan);
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
