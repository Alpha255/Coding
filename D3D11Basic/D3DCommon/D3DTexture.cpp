#include "D3DTexture.h"
#include "D3DGraphic.h"
#include "Utility.h"

#include <DirectXTK/Inc/DDSTextureLoader.h>

extern D3DGraphic* g_Renderer;

void D3DTexture::Create(const char *pFileName)
{
	assert(pFileName && g_Renderer && !IsValid());

	std::string texFilePath = Utility::ResourceFilePath(pFileName, Utility::eTexture);
	std::wstring wtexFilePath(texFilePath.begin(), texFilePath.end());

	ID3D11ShaderResourceView *pTex = nullptr;
	///HRCheck(DirectX::CreateDDSTextureFromFile(g_Renderer->m_D3DDevice.Ptr(), wtexFilePath.c_str(), nullptr, &pTex));

	m_HWTexture.reset(pTex);
}
