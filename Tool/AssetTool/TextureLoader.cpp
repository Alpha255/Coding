#include "AssetTool.h"
#include "Base/AssetFile.h"
#include <d3d11.h>
#include <DirectXTK/Inc/DDSTextureLoader.h>

NamespaceBegin(AssetTool)

void CreateTextureFromFile(
	ID3D11Device* pD3DDevice,
	AssetFile &asset,
	__out ID3D11Resource **pTexture,
	__out ID3D11ShaderResourceView **pTextureView)
{
	assert(pD3DDevice && pTextureView);

	std::string filePath(asset.GetPath());
	std::wstring szFileName(filePath.cbegin(), filePath.cend());

	if (FAILED(DirectX::CreateDDSTextureFromFile(pD3DDevice, szFileName.c_str(), pTexture, pTextureView)))
	{
		assert(0);
	}
}

NamespaceEnd(AssetTool)