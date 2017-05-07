#include "Lighting.h"
#include <RefCountPtr.h>
#include <D3DGraphic.h>

extern D3DGraphic* g_Renderer;

struct DemoLightingResource
{
	Ref<ID3D11VertexShader> VertexShader;
	Ref<ID3D11InputLayout> InputLayout;
	Ref<ID3D11PixelShader> PixelShader;
	Ref<ID3D11Buffer> ConstantsBuffer;
	Ref<ID3D11Buffer> VertexBuffer;
	Ref<ID3D11Buffer> IndexBuffer;

	D3D11_VIEWPORT Viewport;
};

struct ConstantsBufferVS
{
	DirectX::XMMATRIX World;
	DirectX::XMMATRIX WorldInverseTrans;
	DirectX::XMMATRIX WVP;

	DirectX::XMFLOAT4 ViewPoint;
};

struct ConstantsBufferPS
{
	Light::DirectionalLight DirLight;
	Light::PointLight PointLight;
	Light::SpotLight SpotLight;

	Light::Material Material;
};

struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 Normal;
};

static DemoLightingResource s_D3DResource;
static ConstantsBufferVS s_CBufVS;
static ConstantsBufferPS s_CBufPS;
static char* const s_ShaderName = "Lighting.hlsl";

ApplicationLighting::ApplicationLighting()
{

}

void ApplicationLighting::SetupScene()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	g_Renderer->CreateVertexShaderAndInputLayout(s_D3DResource.VertexShader.GetReference(), s_D3DResource.InputLayout.GetReference(),
		layout, ARRAYSIZE(layout), s_ShaderName, "VSMain");
	g_Renderer->CreatePixelShader(s_D3DResource.PixelShader.GetReference(), s_ShaderName, "PSMain");	
}

void ApplicationLighting::RenderScene()
{

}

void ApplicationLighting::UpdateScene(float elapsedTime, float totalTime)
{

}

void ApplicationLighting::ResizeWindow(uint32_t width, uint32_t height)
{

}

void ApplicationLighting::MouseMove(WPARAM wParam, int x, int y)
{

}
