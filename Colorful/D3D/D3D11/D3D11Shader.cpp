#include "D3D11Shader.h"
#include "D3D11Engine.h"

d3d11Shader::d3d11Shader(const d3d11Device &device, eRShaderUsage usage, const rAsset::rShaderBinary &binary)
	: rShader(usage)
{
	assert(!isValid() && device.isValid());
	assert(binary.Binary && binary.Size > 0u && usage < eRShaderUsage_MaxEnum);

	switch (usage)
	{
	case eVertexShader:
	{
		ID3D11VertexShader *pShader = nullptr;
		rVerifyD3D11(device->CreateVertexShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eHullShader:
	{
		ID3D11HullShader *pShader = nullptr;
		rVerifyD3D11(device->CreateHullShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eDomainShader:
	{
		ID3D11DomainShader *pShader = nullptr;
		rVerifyD3D11(device->CreateDomainShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eGeometryShader:
	{
		ID3D11GeometryShader *pShader = nullptr;
		rVerifyD3D11(device->CreateGeometryShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eFragmentShader:
	{
		ID3D11PixelShader *pShader = nullptr;
		rVerifyD3D11(device->CreatePixelShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	case eComputeShader:
	{
		ID3D11ComputeShader *pShader = nullptr;
		rVerifyD3D11(device->CreateComputeShader(binary.Binary.get(), binary.Size, nullptr, &pShader));
		reset(pShader);
	}
		break;
	}
}