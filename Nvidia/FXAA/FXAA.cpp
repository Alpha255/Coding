#include "FXAA.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

#define FakeShader

extern D3DGraphic* g_Renderer;

namespace FXAA
{
	enum
	{
		eGatherCount = 2,
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
#ifndef FakeShader
		Ref<ID3D11VertexShader> MainVS;
		Ref<ID3D11VertexShader> ShadowMapVS;
		Ref<ID3D11VertexShader> FxaaVS;

		Ref<ID3D11PixelShader> MainPS[ePatternCount];
		Ref<ID3D11PixelShader> MainGatherPS[ePatternCount];
		Ref<ID3D11PixelShader> FxaaPS[eFxaaPatternCount];
#else
		Ref<ID3D11VertexShader> FakeMainVS;
		Ref<ID3D11PixelShader> FakeMainPS;
#endif
	};

	struct D3DStates
	{
		Ref<ID3D11BlendState> ColorWritesOn;
		Ref<ID3D11BlendState> ColorWritesOff;

		Ref<ID3D11RasterizerState> CullBack;
		Ref<ID3D11RasterizerState> CullFront;

		Ref<ID3D11RasterizerState> WireFrame;

		Ref<ID3D11SamplerState> PointMirror;    /// For rotation texture
		Ref<ID3D11SamplerState> LinearWrap;     /// For diffuse texture
		Ref<ID3D11SamplerState> PointCmpClamp;  /// Comparison sampler for shadowmap
		Ref<ID3D11SamplerState> Anisotropic;    /// Anisotropic sampler for FXAA
	};

	struct D3DConstantsBuffers
	{
#ifndef FakeShader
		Ref<ID3D11Buffer> Constants;
		Ref<ID3D11Buffer> Fxaa;
#else
		Ref<ID3D11Buffer> Fake;
#endif
	};

	struct D3DStreamBuffer
	{
		Ref<ID3D11Buffer> SphereVB;
		Ref<ID3D11Buffer> SphereIB;
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

	struct ConstantsFake
	{
		DirectX::XMMATRIX MatrixWVP;
	};

	struct Vertex
	{
		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT4 Color;
	};

	struct MatrixSet
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX View;
		DirectX::XMMATRIX Proj;
	};

	static bool s_UseGather = false;
	static uint32_t s_CurPattern = 0U;
	static uint32_t s_FxaaPreset = 4U;
	static uint32_t s_SphereIndexCount = 0U;
	static float s_Radius = 5.0f;
	static float s_Theta = 1.5f * DirectX::XM_PI;
	static float s_Phi = DirectX::XM_PIDIV4;

	static D3DTextures s_Textures;
	static D3DViews s_Views;
	static D3DShaders s_Shaders;
	static D3DStates s_States;
	static D3DConstantsBuffers s_ConstantsBuffers;
	static D3DStreamBuffer s_StreamBuffer;
	static Ref<ID3D11InputLayout> s_InputLayout;
	static D3D11_VIEWPORT s_Viewport;
	static MatrixSet s_MatrixSet;
}

ApplicationFXAA::ApplicationFXAA()
{
	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
	FXAA::s_MatrixSet.World = I;
	FXAA::s_MatrixSet.View = I;
	FXAA::s_MatrixSet.Proj = I;
}

void ApplicationFXAA::CreateTextures()
{
	g_Renderer->CreateTexture2D(FXAA::s_Textures.CopyResolveTex.GetReference(), DXGI_FORMAT_R8G8B8A8_UNORM, 
		m_Width, m_Height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	g_Renderer->CreateTexture2D(FXAA::s_Textures.ProxyTex.GetReference(), DXGI_FORMAT_R8G8B8A8_UNORM,
		m_Width, m_Height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

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

	/// Shader resource view for shadowmap
	assert(FXAA::s_Textures.DepthTex.IsValid());
	g_Renderer->CreateShaderResourceView(FXAA::s_Views.DepthTexSRV.GetReference(), FXAA::s_Textures.DepthTex.GetPtr(), 
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS, D3D11_SRV_DIMENSION_TEXTURE2D);
	g_Renderer->CreateDepthStencilView(FXAA::s_Views.DepthTexDSV.GetReference(), FXAA::s_Textures.DepthTex.GetPtr(),
		DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_DSV_DIMENSION_TEXTURE2D);

	/// Shader resource view for random rotation texture
	assert(FXAA::s_Textures.RandomRotTex.IsValid());
	g_Renderer->CreateShaderResourceView(FXAA::s_Views.RandomRotTexSRV.GetReference(), FXAA::s_Textures.RandomRotTex.GetPtr(),
		DXGI_FORMAT_R8G8B8A8_SNORM, D3D11_SRV_DIMENSION_TEXTURE2D);
}

void ApplicationFXAA::CreateInputLayoutAndShaders()
{
#ifndef FakeShader
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	g_Renderer->CreateVertexShaderAndInputLayout(FXAA::s_Shaders.ShadowMapVS.GetReference(), FXAA::s_InputLayout.GetReference(),
		layout, ARRAYSIZE(layout), "Fxaa.hlsl", "ShadowMapVSMain");

	g_Renderer->CreateVertexShader(FXAA::s_Shaders.MainVS.GetReference(), "Fxaa.hlsl", "VSMain");

	g_Renderer->CreateVertexShader(FXAA::s_Shaders.FxaaVS.GetReference(), "Fxaa.hlsl", "FxaaVSMain");

	/// Pixel shaders
	static D3D_SHADER_MACRO PatternMacros[FXAA::ePatternCount] =
	{
		{ "PATTERN", "44" },
		{ "PATTERN", "64" },
		{ "PATTERN", "38" },
		{ "PATTERN", "84" },
		{ "PATTERN", "48" }
	};
	static D3D_SHADER_MACRO GatherMacros[FXAA::eGatherCount] =
	{
		{ "NOUSEGATHER4", "" },
		{ "USEGATHER4", "" }
	};
	static D3D_SHADER_MACRO PresetMacros[FXAA::eFxaaPatternCount] =
	{
		{ "FXAA_PRESET", "0" },
		{ "FXAA_PRESET", "1" },
		{ "FXAA_PRESET", "2" },
		{ "FXAA_PRESET", "3" },
		{ "FXAA_PRESET", "4" },
		{ "FXAA_PRESET", "5" }
	};
	static D3D_SHADER_MACRO NullMacro = { nullptr, nullptr };

	std::vector<D3D_SHADER_MACRO> Macros;
	for (uint32_t pat = 0U; pat < FXAA::ePatternCount; ++pat)
	{
		Macros.clear();
		Macros.push_back(PatternMacros[pat]);
		Macros.push_back(GatherMacros[0]);
		Macros.push_back(NullMacro);
		g_Renderer->CreatePixelShader(FXAA::s_Shaders.MainPS[pat].GetReference(), "Fxaa.hlsl", "MainPS", (const D3D_SHADER_MACRO*)&*Macros.begin());

		Macros.clear();
		Macros.push_back(PatternMacros[pat]);
		Macros.push_back(GatherMacros[1]);
		Macros.push_back(NullMacro);
		g_Renderer->CreatePixelShader(FXAA::s_Shaders.MainGatherPS[pat].GetReference(), "Fxaa.hlsl", "MainPS", (const D3D_SHADER_MACRO*)&*Macros.begin());
	}

	for (uint32_t pat = 0U; pat < FXAA::eFxaaPatternCount; ++pat)
	{
		Macros.clear();
		Macros.push_back(PresetMacros[pat]);
		g_Renderer->CreatePixelShader(FXAA::s_Shaders.FxaaPS[pat].GetReference(), "Fxaa.hlsl", "FxaaMainPS", (const D3D_SHADER_MACRO*)&*Macros.begin());
	}
#else
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_Renderer->CreateVertexShaderAndInputLayout(FXAA::s_Shaders.FakeMainVS.GetReference(), FXAA::s_InputLayout.GetReference(),
		layout, ARRAYSIZE(layout), "Box.hlsl", "VSMain");
	g_Renderer->CreatePixelShader(FXAA::s_Shaders.FakeMainPS.GetReference(), "Box.hlsl", "PSMain");
#endif
}

void ApplicationFXAA::CreateMesh()
{
	Math::Geometry::Mesh Sphere;
	Math::Geometry::MakeGeoSphere(0.8f, 2U, Sphere);

	///Math::Geometry::Mesh Floor;
	///Math::Geometry::MakeBox(1.6f, 1.0f, 0.2f, Floor);

	std::vector<FXAA::Vertex> Vertices(Sphere.Vertices.size()/* + Floor.Vertices.size()*/);
	for (size_t i = 0U; i < Sphere.Vertices.size(); ++i)
	{
		Vertices[i].Position = Sphere.Vertices[i].Position;
		Vertices[i].Color = Color::LightSteelBlue;
	}

	std::vector<uint32_t> Indices;
	Indices.insert(Indices.end(), Sphere.Indices.begin(), Sphere.Indices.end());

	g_Renderer->CreateVertexBuffer(FXAA::s_StreamBuffer.SphereVB.GetReference(),
		(uint32_t)(sizeof(FXAA::Vertex) * Sphere.Vertices.size()), D3D11_USAGE_IMMUTABLE, &Vertices[0]);

	g_Renderer->CreateIndexBuffer(FXAA::s_StreamBuffer.SphereIB.GetReference(),
		(uint32_t)(sizeof(uint32_t) * Sphere.Indices.size()), D3D11_USAGE_IMMUTABLE, &Indices[0]);

	FXAA::s_SphereIndexCount = (uint32_t)Indices.size();
}

void ApplicationFXAA::CreateStates()
{
	/// Sampler state
	D3D11_SAMPLER_DESC samDesc;
	memset(&samDesc, 0, sizeof(D3D11_SAMPLER_DESC));

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samDesc.MipLODBias = 0.0f;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.BorderColor[0] = samDesc.BorderColor[1] = samDesc.BorderColor[2] = samDesc.BorderColor[3] = 0;
	samDesc.MinLOD = 0;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(FXAA::s_States.PointMirror.GetReference(), &samDesc);

	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	g_Renderer->CreateSamplerState(FXAA::s_States.LinearWrap.GetReference(), &samDesc);

	samDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	g_Renderer->CreateSamplerState(FXAA::s_States.PointCmpClamp.GetReference(), &samDesc);

	samDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samDesc.MaxAnisotropy = 4;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = 0.0f;
	samDesc.MinLOD = 0.0f;
	g_Renderer->CreateSamplerState(FXAA::s_States.Anisotropic.GetReference(), &samDesc);

	/// Blend state
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Renderer->CreateBlendState(FXAA::s_States.ColorWritesOn.GetReference(), &blendDesc);

	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0U;
	g_Renderer->CreateBlendState(FXAA::s_States.ColorWritesOff.GetReference(), &blendDesc);

	/// Rasterizer state
	g_Renderer->CreateRasterizerState(FXAA::s_States.CullBack.GetReference(), D3D11_FILL_SOLID, D3D11_CULL_BACK);

	g_Renderer->CreateRasterizerState(FXAA::s_States.CullFront.GetReference(), D3D11_FILL_SOLID, D3D11_CULL_FRONT);

	g_Renderer->CreateRasterizerState(FXAA::s_States.WireFrame.GetReference(), D3D11_FILL_WIREFRAME);
}

void ApplicationFXAA::CreateConstantsBuffers()
{
#ifndef FakeShader
	g_Renderer->CreateConstantBuffer(FXAA::s_ConstantsBuffers.Constants.GetReference(), D3D11_BIND_CONSTANT_BUFFER,
		sizeof(FXAA::ConstantsBuf), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->CreateConstantBuffer(FXAA::s_ConstantsBuffers.Fxaa.GetReference(), D3D11_BIND_CONSTANT_BUFFER,
		sizeof(FXAA::ConstantsFxaa), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
#else
	g_Renderer->CreateConstantBuffer(FXAA::s_ConstantsBuffers.Fake.GetReference(),
		sizeof(FXAA::ConstantsFake), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
#endif
}

void ApplicationFXAA::SetupScene()
{
	if (m_bInited)
	{
		return;
	}

	CreateTextures();
	CreateViews();
	CreateInputLayoutAndShaders();
	CreateMesh();
	CreateStates();
	CreateConstantsBuffers();
	SetupD3D();

	m_bInited = true;
}

void ApplicationFXAA::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), nullptr);
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), 1.0f, 0U);

#ifndef FakeShader
#else
	g_Renderer->SetVertexBuffer(FXAA::s_StreamBuffer.SphereVB, sizeof(FXAA::Vertex), 0U);
	g_Renderer->SetIndexBuffer(FXAA::s_StreamBuffer.SphereIB, DXGI_FORMAT_R32_UINT);
	
	FXAA::ConstantsFake FakeConstants;
	FakeConstants.MatrixWVP = DirectX::XMMatrixTranspose(FXAA::s_MatrixSet.World * FXAA::s_MatrixSet.View * FXAA::s_MatrixSet.Proj);

	g_Renderer->SetConstantBuffer(FXAA::s_ConstantsBuffers.Fake, 0U, D3DGraphic::eSTVertexShader);
	g_Renderer->UpdateConstantBuffer(FXAA::s_ConstantsBuffers.Fake.GetPtr(), &FakeConstants, sizeof(FXAA::ConstantsFake));

	g_Renderer->SetRasterizerState(FXAA::s_States.WireFrame);
	g_Renderer->DrawIndexed(FXAA::s_SphereIndexCount, 0U, 0);
#endif
}

void ApplicationFXAA::SetupD3D()
{
	g_Renderer->SetInputLayout(FXAA::s_InputLayout);

#ifndef FakeShader
#else
	g_Renderer->SetVertexShader(FXAA::s_Shaders.FakeMainVS);
	g_Renderer->SetPixelShader(FXAA::s_Shaders.FakeMainPS);
#endif

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	FXAA::s_Viewport.Width = (float)m_Width;
	FXAA::s_Viewport.Height = (float)m_Height;
	FXAA::s_Viewport.MinDepth = 0.0f;
	FXAA::s_Viewport.MaxDepth = 1.0f;
	FXAA::s_Viewport.TopLeftX = FXAA::s_Viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&FXAA::s_Viewport);
}

void ApplicationFXAA::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = FXAA::s_Radius * sinf(FXAA::s_Phi) * cosf(FXAA::s_Theta);
	float z = FXAA::s_Radius * sinf(FXAA::s_Phi) * sinf(FXAA::s_Theta);
	float y = FXAA::s_Radius * cosf(FXAA::s_Phi);

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	FXAA::s_MatrixSet.View = DirectX::XMMatrixLookAtLH(pos, target, up);
}

void ApplicationFXAA::ResizeWindow(uint32_t width, uint32_t height)
{
	FXAA::s_MatrixSet.Proj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (FLOAT)width / height, 1.0f, 100.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationFXAA::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		FXAA::s_Theta += dx;
		FXAA::s_Phi += dy;

		FXAA::s_Phi = Math::Clamp(FXAA::s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		float dx = 0.0051f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.0051f * static_cast<float>(y - m_LastMousePos[1]);

		FXAA::s_Radius += dx - dy;

		FXAA::s_Radius = Math::Clamp(FXAA::s_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}