#include "Box.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

extern D3DGraphic* g_Renderer;

NamespaceBegin()
struct MatrixSet
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 View;
	DirectX::XMFLOAT4X4 Projection;
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
};

struct Constants
{
	DirectX::XMMATRIX WVP;
};

struct D3DResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11InputLayout> InputLayout;
	Ref<ID3D11PixelShader> PixelShader;
	Ref<ID3D11Buffer> ConstantsBuffer;
	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;
};

static float s_Radius = 5.0f;
static float s_Phi = DirectX::XM_PIDIV4;
static float s_Theta = 1.5f * DirectX::XM_PI;
static MatrixSet s_MatrixSet;
static D3DResource s_D3DResource;
static char* s_ShaderName = "Box.hlsl";
NamespaceEnd()

ApplicationBox::ApplicationBox(HINSTANCE hInstance, LPCWSTR lpTitle)
	: Base(hInstance, lpTitle)
	, m_bInited(false)
{
	DirectX::XMMATRIX I = DirectX::XMMatrixIdentity();
	DirectX::XMStoreFloat4x4(&s_MatrixSet.World, I);
	DirectX::XMStoreFloat4x4(&s_MatrixSet.View, I);
	DirectX::XMStoreFloat4x4(&s_MatrixSet.Projection, I);
}

void ApplicationBox::Setup()
{
	assert(g_Renderer && !m_bInited);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_Renderer->CreateVertexShaderAndInputLayout(s_D3DResource.VertexShader.GetReference(), s_D3DResource.InputLayout.GetReference(), 
		layout, ARRAYSIZE(layout), s_ShaderName, "VSMain");
	g_Renderer->CreatePixelShader(s_D3DResource.PixelShader.GetReference(), s_ShaderName, "PSMain");
	g_Renderer->CreateStreamBuffer(s_D3DResource.ConstantsBuffer.GetReference(), D3D11_BIND_CONSTANT_BUFFER, sizeof(Constants),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	Vertex vertices[] =
	{
		{ DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), Color::White },
		{ DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), Color::Black },
		{ DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), Color::Red },
		{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), Color::Green },
		{ DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), Color::Blue },
		{ DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), Color::Yellow },
		{ DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), Color::Cyan },
		{ DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), Color::Magenta }
	};
	g_Renderer->CreateStreamBuffer(s_D3DResource.VertexBuffer.GetReference(), D3D11_BIND_VERTEX_BUFFER, sizeof(Vertex) * 8U,
		D3D11_USAGE_IMMUTABLE, vertices);

	uint32_t indices[] = {
		/// front face
		0, 1, 2,
		0, 2, 3,
		/// back face
		4, 6, 5,
		4, 7, 6,
		/// left face
		4, 5, 1,
		4, 1, 0,
		/// right face
		3, 2, 6,
		3, 6, 7,
		/// top face
		1, 5, 6,
		1, 6, 2,
		/// bottom face
		4, 0, 3,
		4, 3, 7
	};
	g_Renderer->CreateStreamBuffer(s_D3DResource.IndexBuffer.GetReference(), D3D11_BIND_INDEX_BUFFER, sizeof(uint32_t) * 36,
		D3D11_USAGE_IMMUTABLE, indices);
	
	g_Renderer->SetVertexShader(s_D3DResource.VertexShader.GetPtr());
	g_Renderer->SetPixelShader(s_D3DResource.PixelShader.GetPtr());
	g_Renderer->SetVertexBuffer(s_D3DResource.VertexBuffer.GetPtr(), sizeof(Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_D3DResource.IndexBuffer.GetPtr(), DXGI_FORMAT_R32_UINT);
	g_Renderer->SetInputLayout(s_D3DResource.InputLayout.GetPtr());

	//g_pGraphic->SetOutputRenderTarget();
	//g_pGraphic->SetOutputDepthStencil();
	//g_pGraphic->SetDefaultViewport();

	m_bInited = true;
}

void ApplicationBox::RenderScene()
{

}

void ApplicationBox::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	if (!m_bInited)
	{
		Setup();
	}
}

void ApplicationBox::ResizeWindow(uint32_t width, uint32_t height)
{

}

void ApplicationBox::MouseMove(WPARAM wParam, int x, int y)
{

}