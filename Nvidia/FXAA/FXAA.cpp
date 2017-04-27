#include "FXAA.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

extern D3DGraphic* g_Renderer;

namespace FXAA
{
	enum
	{
		ePatternCount = 5,
		eFxaaPatternCount = 6,
		eShadowMapSize = 2048,
		eRandomRotSize = 16
	};

	struct D3DTextures
	{
		Ref<ID3D11Texture2D> ProxyTex;
		Ref<ID3D11Texture2D> DepthTex;
		Ref<ID3D11Texture2D> RandomRotTex;
		Ref<ID3D11Texture2D> CopyResolveTex;
	};

	struct D3DViews
	{
		Ref<ID3D11ShaderResourceView> ProxyTexSRV;
		Ref<ID3D11ShaderResourceView> DepthTexSRV;
		Ref<ID3D11ShaderResourceView> RandomRotTexSRV;
		Ref<ID3D11ShaderResourceView> CopyResolveTexSRV;

		Ref<ID3D11DepthStencilView> DepthTexDSV;
		Ref<ID3D11RenderTargetView> ProxyTexRTV;
	};

	struct D3DShaders
	{
		Ref<ID3D11VertexShader> MainVS;
		Ref<ID3D11VertexShader> ShadowMapVS;
		Ref<ID3D11VertexShader> FxaaVS;

		Ref<ID3D11PixelShader> MainPS[ePatternCount];
		Ref<ID3D11PixelShader> MainGatherPS[ePatternCount];
		Ref<ID3D11PixelShader> FxaaPS[eFxaaPatternCount];
	};

	struct D3DStates
	{
		Ref<ID3D11BlendState> ColorWritesOn;
		Ref<ID3D11BlendState> ColorWritesOff;

		Ref<ID3D11RasterizerState> CullBack;
		Ref<ID3D11RasterizerState> CullFront;

		Ref<ID3D11SamplerState> PointMirror;    /// For rotation texture
		Ref<ID3D11SamplerState> LinearWrap;     /// For diffuse texture
		Ref<ID3D11SamplerState> PointCmpClamp;  /// Comparison sampler for shadowmap
		Ref<ID3D11SamplerState> Anisotropic;    /// Anisotropic sampler for FXAA
	};

	struct D3DConstantsBuffers
	{
		Ref<ID3D11Buffer> Constants;
		Ref<ID3D11Buffer> Fxaa;
	};

	struct ConstantsBuf
	{
		DirectX::XMMATRIX WVP;
		DirectX::XMMATRIX WVPLight;

		DirectX::XMFLOAT4 AmbientColor;
		DirectX::XMFLOAT4 DiffuseColor;

		DirectX::XMFLOAT4 LightPos;
		DirectX::XMFLOAT4 LightDiffuseClr;

		float InvRandomRotSize;
		float InvShadowMapSize;
		float FilterWidth;
		float Padding;
	};

	struct ConstantsFxaa
	{
		DirectX::XMFLOAT4 Fxaa;
	};

	static bool s_UseGather = false;
	static uint32_t s_CurPattern = 0U;
	static uint32_t s_FxaaPreset = 4U;

	static D3DTextures s_Textures;
	static D3DViews s_Views;
}

ApplicationFXAA::ApplicationFXAA(HINSTANCE hInstance, LPCWSTR lpTitle)
	: Base(hInstance, lpTitle)
{

}

void ApplicationFXAA::CreateTextures()
{
	g_Renderer->CreateTexture2D(FXAA::s_Textures.CopyResolveTex.GetReference(), DXGI_FORMAT_R8G8B8A8_UNORM, 
		m_Size[0], m_Size[1], D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	g_Renderer->CreateTexture2D(FXAA::s_Textures.ProxyTex.GetReference(), DXGI_FORMAT_R8G8B8A8_UNORM,
		m_Size[0], m_Size[1], D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	/// Shadow map surfaces
	g_Renderer->CreateTexture2D(FXAA::s_Textures.DepthTex.GetReference(), DXGI_FORMAT_R24G8_TYPELESS,
		FXAA::eShadowMapSize, FXAA::eShadowMapSize, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);

	/// Random rotation texture surface 
	uint32_t texData[FXAA::eRandomRotSize * FXAA::eRandomRotSize] = { 0 };
	for (uint32_t i = 0; i < FXAA::eRandomRotSize * FXAA::eRandomRotSize; ++i)
	{
		float angle = (float)rand() / RAND_MAX * DirectX::XM_2PI;

		float cosAngle = cosf(angle);
		float sinAngle = sinf(angle);

		int x1 = (int)(127.0f * cosAngle);
		int y1 = (int)(127.0f * sinAngle);
		int x2 = (int)(-127.0f * sinAngle);
		int y2 = (int)(127.0f * cosAngle);

		texData[i] = (uint32_t)(((x1 & 0xff) << 24) | ((y1 & 0xff) << 16) | ((x2 & 0xff) << 8) | (y2 & 0xff));
	}
	g_Renderer->CreateTexture2D(FXAA::s_Textures.RandomRotTex.GetReference(), DXGI_FORMAT_R8G8B8A8_SNORM,
		FXAA::eRandomRotSize, FXAA::eRandomRotSize, D3D11_BIND_SHADER_RESOURCE, 1U, 1U, 0U, 1U, 0U, 0U, D3D11_USAGE_DEFAULT, 
		texData, sizeof(uint32_t) * FXAA::eRandomRotSize);
}

void ApplicationFXAA::CreateViews()
{
	assert(FXAA::s_Textures.ProxyTex.IsValid());
	g_Renderer->CreateRenderTargetView(FXAA::s_Views.ProxyTexRTV.GetReference(), FXAA::s_Textures.ProxyTex.GetPtr());
	g_Renderer->CreateShaderResourceView(FXAA::s_Views.ProxyTexSRV.GetReference(), FXAA::s_Textures.ProxyTex.GetPtr());

	assert(FXAA::s_Textures.CopyResolveTex.IsValid());
	g_Renderer->CreateShaderResourceView(FXAA::s_Views.CopyResolveTexSRV.GetReference(), FXAA::s_Textures.CopyResolveTex.GetPtr());

	/// Depth stencil view for shadowmap
}

void ApplicationFXAA::CreateShaders()
{

}

void ApplicationFXAA::CreateSamplerStates()
{

}

void ApplicationFXAA::CreateBlendStates()
{

}

void ApplicationFXAA::CreateRasterizerStates()
{

}

void ApplicationFXAA::CreateConstantsBuffers()
{

}

void ApplicationFXAA::SetupScene()
{
	if (m_bInited)
	{
		return;
	}

	CreateTextures();
	CreateViews();
	CreateShaders();
	CreateSamplerStates();
	CreateBlendStates();
	CreateRasterizerStates();
	CreateConstantsBuffers();

	m_bInited = true;
}

void ApplicationFXAA::RenderScene()
{

}

void ApplicationFXAA::UpdateScene(float elapsedTime, float totalTime)
{

}

void ApplicationFXAA::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);
}

void ApplicationFXAA::MouseMove(WPARAM wParam, int x, int y)
{

}