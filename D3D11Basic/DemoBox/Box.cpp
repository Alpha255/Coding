#include "Box.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

extern D3DGraphic* g_Renderer;

struct MatrixSet
{
	Matrix World;
	Matrix View;
	Matrix Projection;
};

struct Vertex
{
	Vec3 Position;
	Vec4 Color;
};

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

	D3D11_VIEWPORT Viewport;
};

static float s_Radius = 5.0f;
static float s_Phi = DirectX::XM_PIDIV4;
static float s_Theta = 1.5f * DirectX::XM_PI;
static MatrixSet s_MatrixSet;
static DemoBoxResource s_D3DResource;
static char* const s_ShaderName = "Box.hlsl";

ApplicationBox::ApplicationBox()
{
	s_MatrixSet.World.Identity();
	s_MatrixSet.View.Identity();
	s_MatrixSet.Projection.Identity();
}

void ApplicationBox::SetupScene()
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
	g_Renderer->CreateConstantBuffer(s_D3DResource.ConstantsBuffer.GetReference(), sizeof(Constants),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

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
	g_Renderer->CreateVertexBuffer(s_D3DResource.VertexBuffer.GetReference(), sizeof(Vertex) * 8U,
		D3D11_USAGE_IMMUTABLE, vertices);

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
	g_Renderer->CreateIndexBuffer(s_D3DResource.IndexBuffer.GetReference(), sizeof(uint32_t) * 36,
		D3D11_USAGE_IMMUTABLE, indices);
	
	g_Renderer->SetVertexShader(s_D3DResource.VertexShader);
	g_Renderer->SetPixelShader(s_D3DResource.PixelShader);
	g_Renderer->SetVertexBuffer(s_D3DResource.VertexBuffer, sizeof(Vertex), 0U);
	g_Renderer->SetIndexBuffer(s_D3DResource.IndexBuffer, DXGI_FORMAT_R32_UINT);
	g_Renderer->SetInputLayout(s_D3DResource.InputLayout);

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
	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget(), black);
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), 1.0f, 0U);

	Matrix wvp = s_MatrixSet.World * s_MatrixSet.View * s_MatrixSet.Projection;

	Constants cBuffer;
	memset(&cBuffer, 0, sizeof(Constants));
	cBuffer.WVP = wvp.Transpose();
	g_Renderer->UpdateConstantBuffer(s_D3DResource.ConstantsBuffer.GetPtr(), &cBuffer, sizeof(Constants));
	g_Renderer->SetConstantBuffer(s_D3DResource.ConstantsBuffer, 0U, D3DGraphic::eVertexShader);

	g_Renderer->DrawIndexed(36U, 0U, 0U);
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