#include "Box.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

extern D3DGraphic* g_Renderer;

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

	D3D11_VIEWPORT Viewport;
};

static float s_Radius = 5.0f;
static float s_Phi = DirectX::XM_PIDIV4;
static float s_Theta = 1.5f * DirectX::XM_PI;
static MatrixSet s_MatrixSet;
static D3DResource s_D3DResource;
static char* s_ShaderName = "Box.hlsl";

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

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	s_D3DResource.Viewport.Width = (float)m_Size[0];
	s_D3DResource.Viewport.Height = (float)m_Size[1];
	s_D3DResource.Viewport.MinDepth = 0.0f;
	s_D3DResource.Viewport.MaxDepth = 1.0f;
	s_D3DResource.Viewport.TopLeftX = s_D3DResource.Viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&s_D3DResource.Viewport);

	m_bInited = true;
}

void ApplicationBox::RenderScene()
{
	float black[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), nullptr);
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), 1.0f, 0U);

	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&s_MatrixSet.World);
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&s_MatrixSet.View);
	DirectX::XMMATRIX projection = DirectX::XMLoadFloat4x4(&s_MatrixSet.Projection);
	DirectX::XMMATRIX wvp = world * view * projection;

	Constants cBuffer;
	memset(&cBuffer, 0, sizeof(Constants));
	cBuffer.WVP = DirectX::XMMatrixTranspose(wvp);
	g_Renderer->UpdateConstantBuffer(s_D3DResource.ConstantsBuffer.GetPtr(), &cBuffer, sizeof(Constants));
	g_Renderer->SetConstantBuffer(s_D3DResource.ConstantsBuffer, 0U, D3DGraphic::eSTVertexShader);

	g_Renderer->DrawIndexed(36U, 0U, 0U);
}

void ApplicationBox::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	if (!m_bInited)
	{
		Setup();
	}

	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX lookAt = DirectX::XMMatrixLookAtLH(pos, target, up);

	DirectX::XMStoreFloat4x4(&s_MatrixSet.View, lookAt);
}

void ApplicationBox::ResizeWindow(uint32_t width, uint32_t height)
{
	DirectX::XMMATRIX perspective = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 100.0f);
	DirectX::XMStoreFloat4x4(&s_MatrixSet.Projection, perspective);

	Base::ResizeWindow(width, height);

	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	DirectX::XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.0f);
	DirectX::XMVECTOR target = DirectX::XMVectorZero();
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX lookAt = DirectX::XMMatrixLookAtLH(pos, target, up);

	DirectX::XMStoreFloat4x4(&s_MatrixSet.View, lookAt);
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
		float dy = 0.005f * static_cast<float>(y - m_LastMousePos[0]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 15.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}