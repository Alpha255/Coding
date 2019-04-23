#include "AssetTool.h"
#include "Base/AssetFile.h"
#include <d3d11.h>
#include <DirectXTK/Inc/DDSTextureLoader.h>
#include <gli/gli/gli.hpp>

NamespaceBegin(AssetTool)

void CreateTextureFromFile(
	ID3D11Device* pD3DDevice,
	const std::string &fileName,
	__out ID3D11Resource **pTexture,
	__out ID3D11ShaderResourceView **pTextureView)
{
	assert(pD3DDevice && pTextureView);

	AssetFile file(fileName);
	std::string filePath(file.GetPath());
	std::wstring szFileName(filePath.cbegin(), filePath.cend());

	if (FAILED(DirectX::CreateDDSTextureFromFile(pD3DDevice, szFileName.c_str(), pTexture, pTextureView)))
	{
		assert(0);
	}
}

gli::texture *CreateTextureFromFile(const std::string &fileName)
{
	gli::texture *pTexture = new gli::texture();

	AssetFile file(fileName);
	*pTexture = gli::load(file.GetPath());

	return pTexture;
}

NamespaceEnd(AssetTool)