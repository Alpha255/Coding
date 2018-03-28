#include "D3DTexture.h"
#include "D3DGraphic.h"
#include "System.h"

#include <DirectXTK/Inc/DDSTextureLoader.h>

extern D3DGraphic* g_Renderer;

void D3DTexture::CreateFromDDS(const char *pFileName)
{
	assert(pFileName && g_Renderer && !IsValid());

	std::string texFilePath = System::ResourceFilePath(pFileName, System::eTexture);
	std::wstring wtexFilePath(texFilePath.begin(), texFilePath.end());

	ID3D11ShaderResourceView *pTex = nullptr;
	///HRCheck(DirectX::CreateDDSTextureFromFile(g_Renderer->m_D3DDevice.Ptr(), wtexFilePath.c_str(), nullptr, &pTex));

	m_Resource.reset(pTex);
}
