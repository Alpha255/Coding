#include "Box.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

#define UsingTexture

extern D3DGraphic* g_Renderer;

struct MatrixSet
{
	Matrix World;
	Matrix View;
	Matrix Projection;
};

#ifdef UsingTexture
struct Vertex
{
	Vec3 Position;
	Vec2 UV;
};
#else
struct Vertex
{
	Vec3 Position;
	Vec4 Color;
};
#endif

struct Constants
{
	Matrix WVP;
};

struct DemoBoxResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11InputLayout> InputLayout;
	Ref<ID3D11PixelShader> PixelShader;
	Ref<ID3D11Buffer> ConstantsBuffer;
	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

#ifdef UsingTexture
	Ref<ID3D11ShaderResourceView> Texture;
	Ref<ID3D11SamplerState> Sampler;
#endif

	D3D11_VIEWPORT Viewport;
};

static float s_Radius = 5.0f;
static float s_Phi = DirectX::XM_PIDIV4;
static float s_Theta = 1.5f * DirectX::XM_PI;
static MatrixSet s_MatrixSet;
static DemoBoxResource s_D3DResource;

static uint32_t s_IndexCount = 36U;
static uint32_t s_IndexOffset = 0U;
static uint32_t s_VertexOffset = 0U;

#ifdef UsingTexture
	static char* const s_ShaderName = "BoxTexture.hlsl";
#else
	static char* const s_ShaderName = "Box.hlsl";
#endif

ApplicationBox::ApplicationBox()
{
	s_MatrixSet.World.Identity();
	s_MatrixSet.View.Identity();
	s_MatrixSet.Projection.Identity();
}

void ApplicationBox::SetupScene()
{
	assert(g_Renderer && !m_bInited);

#ifdef UsingTexture
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Math::Geometry::Mesh box;
	Math::Geometry::MakeBox(1.0f, 1.0f, 1.0f, box);

	std::vector<Vertex> vertices(box.Vertices.size());
	uint32_t k = 0U;
	for (size_t i = 0U; i < box.Vertices.size(); ++i, ++k)
	{
		vertices.at(k).Position = box.Vertices.at(i).Position;
		vertices.at(k).UV = box.Vertices.at(i).UV;
	}

	std::vector<uint32_t> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

	s_IndexCount = (uint32_t)box.Indices.size();

	g_Renderer->CreateShaderResourceView(s_D3DResource.Texture.Reference(), "WoodCrate01.dds");
	
	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_D3DResource.Sampler.Reference(), &sampDesc);

	g_Renderer->CreateVertexBuffer(s_D3DResource.VertexBuffer.Reference(), (uint32_t)(sizeof(Vertex) * box.Vertices.size()),
		D3D11_USAGE_IMMUTABLE, &vertices[0]);
	g_Renderer->CreateIndexBuffer(s_D3DResource.IndexBuffer.Reference(), (uint32_t)(sizeof(uint32_t) * box.Indices.size()),
		D3D11_USAGE_IMMUTABLE, &indices[0]);
#else
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Vertex vertices[] =
	{
		{ Vec3(-1.0f, -1.0f, -1.0f), Color::White },
		{ Vec3(-1.0f,  1.0f, -1.0f), Color::Black },
		{ Vec3(1.0f,  1.0f, -1.0f), Color::Red },
		{ Vec3(1.0f, -1.0f, -1.0f), Color::Green },
		{ Vec3(-1.0f, -1.0f,  1.0f), Color::Blue },
		{ Vec3(-1.0f,  1.0f,  1.0f), Color::Yellow },
		{ Vec3(1.0f,  1.0f,  1.0f), Color::Cyan },
		{ Vec3(1.0f, -1.0f,  1.0f), Color::Magenta }
	};

	uint32_t indices[] = 
	{
		/// Front face
		0, 1, 2,
		0, 2, 3,
		/// Back face
		4, 6, 5,
		4, 7, 6,
		/// Left face
		4, 5, 1,
		4, 1, 0,
		/// Right face
		3, 2, 6,
		3, 6, 7,
		/// Top face
		1, 5, 6,
		1, 6, 2,
		/// Bottom face
		4, 0, 3,
		4, 3, 7
	};

	g_Renderer->CreateVertexBuffer(s_D3DResource.VertexBuffer.Reference(), sizeof(Vertex) * 8U,
		D3D11_USAGE_IMMUTABLE, vertices);
	g_Renderer->CreateIndexBuffer(s_D3DResource.IndexBuffer.Reference(), sizeof(uint32_t) * 36,
		D3D11_USAGE_IMMUTABLE, indices);
#endif

	g_Renderer->CreateVertexShaderAndInputLayout(s_D3DResource.VertexShader.Reference(), s_D3DResource.InputLayout.Reference(),
		layout, ARRAYSIZE(layout), s_ShaderName, "VSMain");
	g_Renderer->CreatePixelShader(s_D3DResource.PixelShader.Reference(), s_ShaderName, "PSMain");
	g_Renderer->CreateConstantBuffer(s_D3DResource.ConstantsBuffer.Reference(), sizeof(Constants),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	g_Renderer->SetVertexShader(s_D3DResource.VertexShader.Ptr());
	g_Renderer->SetPixelShader(s_D3DResource.PixelShader.Ptr());
	g_Renderer->SetVertexBuffer(s_D3DResource.VertexBuffer.Ptr(), sizeof(Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_D3DResource.IndexBuffer.Ptr(), DXGI_FORMAT_R32_UINT);
	g_Renderer->SetInputLayout(s_D3DResource.InputLayout.Ptr());

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	s_D3DResource.Viewport.Width = (float)m_Width;
	s_D3DResource.Viewport.Height = (float)m_Height;
	s_D3DResource.Viewport.MinDepth = 0.0f;
	s_D3DResource.Viewport.MaxDepth = 1.0f;
	s_D3DResource.Viewport.TopLeftX = s_D3DResource.Viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&s_D3DResource.Viewport);

	m_bInited = true;
}

void ApplicationBox::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), reinterpret_cast<const float*>(&Color::Black));
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	Matrix wvp = s_MatrixSet.World * s_MatrixSet.View * s_MatrixSet.Projection;

	Constants cBuffer;
	memset(&cBuffer, 0, sizeof(Constants));
	cBuffer.WVP = wvp.Transpose();
	g_Renderer->UpdateConstantBuffer(s_D3DResource.ConstantsBuffer.Ptr(), &cBuffer, sizeof(Constants));
	g_Renderer->SetConstantBuffer(s_D3DResource.ConstantsBuffer.Ptr(), 0U, D3DGraphic::eVertexShader);

#ifdef UsingTexture
	g_Renderer->SetShaderResource(s_D3DResource.Texture.Reference(), 0U, 1U);
	g_Renderer->SetSamplerStates(s_D3DResource.Sampler.Reference(), 0U, 1U);
#endif

	g_Renderer->DrawIndexed(s_IndexCount, s_IndexOffset, s_VertexOffset);
}

void ApplicationBox::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	s_MatrixSet.View = Matrix::LookAtLH(Vec3(x, y, z), Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
}

void ApplicationBox::ResizeWindow(uint32_t width, uint32_t height)
{
	s_MatrixSet.Projection = Matrix::PerspectiveFovLH(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 100.0f);

	return Base::ResizeWindow(width, height);
}

void ApplicationBox::MouseMove(WPARAM wParam, int x, int y)
{
	if ((wParam & MK_LBUTTON) != 0)
	{
		float dx = DirectX::XMConvertToRadians(0.25f * static_cast<float>(x - m_LastMousePos[0]));
		float dy = DirectX::XMConvertToRadians(0.25f * static_cast<float>(y - m_LastMousePos[1]));

		s_Theta += dx;
		s_Phi += dy;

		s_Phi = Math::Clamp(s_Phi, 0.1f, DirectX::XM_PI - 0.1f);
	}
	else if ((wParam & MK_RBUTTON) != 0)
	{
		float dx = 0.005f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}