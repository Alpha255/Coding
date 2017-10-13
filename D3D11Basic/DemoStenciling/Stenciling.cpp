#include "Stenciling.h"
#include "RefCountPtr.h"
#include "D3DGraphic.h"
#include "D3DModel.h"
#include "D3DMath.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct BasicVertex
{
	Vec3 Pos;
	Vec3 Normal;
	Vec2 UV;

	BasicVertex() = default;
	BasicVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: Pos(x, y, z)
		, Normal(nx, ny, nz)
		, UV(u, v)
	{
	}
};

struct SceneRoomVertices
{
	BasicVertex Floor[6];
	BasicVertex Mirror[6];
	BasicVertex Wall[18];

	Ref<ID3D11InputLayout> Layout;

	Ref<ID3D11Buffer> VBFloor;
	Ref<ID3D11Buffer> VBMirror;
	Ref<ID3D11Buffer> VBWall;

	Ref<ID3D11Buffer> CBVS;
	Ref<ID3D11Buffer> CBPS;
};

struct DemoStencilingResource
{
	Ref<ID3D11ShaderResourceView> FloorDiffuseTex;
	Ref<ID3D11ShaderResourceView> WallDiffuseTex;
	Ref<ID3D11ShaderResourceView> MirrorDiffuseTex;

	Ref<ID3D11VertexShader> VS;
	Ref<ID3DBlob> VSBlob;
	Ref<ID3D11PixelShader> PS;

	SimpleMesh SkullMesh;
	SceneRoomVertices RoomMesh;
};

struct ConstantsBufferVS
{
	Matrix World;
	Matrix WVP;
};

struct ConstantsBufferPS
{
	Lighting::Material MatRoom;
};

static DemoStencilingResource s_Resource;
static ConstantsBufferVS s_CBVS;
static ConstantsBufferPS s_CBPS;
static float s_Radius = 12.0f;
static float s_Phi = DirectX::XM_PI * 0.42f;
static float s_Theta = DirectX::XM_PI * 1.24f;
static Camera s_Camera;
static char* const s_ShaderName = "Stenciling.hlsl";

ApplicationStenciling::ApplicationStenciling()
{
	s_CBPS.MatRoom.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_CBPS.MatRoom.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_CBPS.MatRoom.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);
}

void ApplicationStenciling::SetupScene()
{
	/// Create and specify geometry.  For this sample we draw a floor
	/// and a wall with a mirror on it.  We put the floor, wall, and
	/// mirror geometry in one vertex buffer.
	///
	///   |--------------|
	///   |              |
	///   |----|----|----|
	///   |Wall|Mirr|Wall|
	///   |    | or |    |
	///   /--------------/
	///  /   Floor      /
	/// /--------------/

	g_Renderer->CreateShaderResourceView(s_Resource.FloorDiffuseTex.Reference(), "checkboard.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.WallDiffuseTex.Reference(), "brick01.dds");
	g_Renderer->CreateShaderResourceView(s_Resource.MirrorDiffuseTex.Reference(), "ice.dds");

	g_Renderer->CreateVertexShader(s_Resource.VS.Reference(), s_Resource.VSBlob.Reference(), s_ShaderName, "VS_Main");
	g_Renderer->CreatePixelShader(s_Resource.PS.Reference(), s_ShaderName, "PS_Main");

	s_Resource.RoomMesh.Floor[0] = { -3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f };
	s_Resource.RoomMesh.Floor[1] = { -3.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f };
	s_Resource.RoomMesh.Floor[2] = {  7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f };
	s_Resource.RoomMesh.Floor[3] = { -3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f };
	s_Resource.RoomMesh.Floor[4] = {  7.5f, 0.0f,   0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f };
	s_Resource.RoomMesh.Floor[5] = {  7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f };
	
	s_Resource.RoomMesh.Wall[0]  = { -3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f };
	s_Resource.RoomMesh.Wall[1]  = { -3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f };
	s_Resource.RoomMesh.Wall[2]  = { -2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f };
	s_Resource.RoomMesh.Wall[3]  = { -3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f };
	s_Resource.RoomMesh.Wall[4]  = { -2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f };
	s_Resource.RoomMesh.Wall[5]  = { -2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f };
	s_Resource.RoomMesh.Wall[6]  = {  2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f };
	s_Resource.RoomMesh.Wall[7]  = {  2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f };
	s_Resource.RoomMesh.Wall[8]  = {  7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f };
	s_Resource.RoomMesh.Wall[9]  = {  2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f };
	s_Resource.RoomMesh.Wall[10] = {  7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f };
	s_Resource.RoomMesh.Wall[11] = {  7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f };
	s_Resource.RoomMesh.Wall[12] = { -3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	s_Resource.RoomMesh.Wall[13] = { -3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f };
	s_Resource.RoomMesh.Wall[14] = {  7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f };
	s_Resource.RoomMesh.Wall[15] = { -3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	s_Resource.RoomMesh.Wall[16] = {  7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f };
	s_Resource.RoomMesh.Wall[17] = {  7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f };

	s_Resource.RoomMesh.Mirror[0] = { -2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	s_Resource.RoomMesh.Mirror[1] = { -2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f };
	s_Resource.RoomMesh.Mirror[2] = {  2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f };
	s_Resource.RoomMesh.Mirror[3] = { -2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f };
	s_Resource.RoomMesh.Mirror[4] = {  2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f };
	s_Resource.RoomMesh.Mirror[5] = {  2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f };

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateInputLayout(s_Resource.RoomMesh.Layout.Reference(), layout, ARRAYSIZE(layout), s_Resource.VSBlob.Ptr());

	g_Renderer->CreateVertexBuffer(s_Resource.RoomMesh.VBFloor.Reference(), sizeof(BasicVertex) * 6U, 
		D3D11_USAGE_IMMUTABLE, s_Resource.RoomMesh.Floor);
	g_Renderer->CreateVertexBuffer(s_Resource.RoomMesh.VBWall.Reference(), sizeof(BasicVertex) * 18U, 
		D3D11_USAGE_IMMUTABLE, s_Resource.RoomMesh.Wall);
	g_Renderer->CreateVertexBuffer(s_Resource.RoomMesh.VBMirror.Reference(), sizeof(BasicVertex) * 6U, 
		D3D11_USAGE_IMMUTABLE, s_Resource.RoomMesh.Mirror);

	g_Renderer->CreateConstantBuffer(s_Resource.RoomMesh.CBVS.Reference(), sizeof(ConstantsBufferVS), 
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	g_Renderer->CreateConstantBuffer(s_Resource.RoomMesh.CBPS.Reference(), sizeof(ConstantsBufferPS),
		D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	s_Resource.SkullMesh.CreateFromTxt("skull.txt", nullptr);

	g_Renderer->SetVertexShader(s_Resource.VS.Ptr());
	g_Renderer->SetPixelShader(s_Resource.PS.Ptr());

	g_Renderer->SetRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->SetDepthStencil(g_Renderer->DefaultDepthStencil());

	D3D11_VIEWPORT viewport;
	viewport.Width = (float)m_Width;
	viewport.Height = (float)m_Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = viewport.TopLeftY = 0.0f;
	g_Renderer->SetViewports(&viewport);

	m_bInited = true;
}

void ApplicationStenciling::RenderScene()
{
	g_Renderer->ClearRenderTarget(g_Renderer->DefaultRenderTarget());
	g_Renderer->ClearDepthStencil(g_Renderer->DefaultDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0U);

	/// Draw floor and walls
	Matrix wvp = s_Camera.GetWorldMatrix() * s_Camera.GetViewMatrix() * s_Camera.GetProjMatrix();
	s_CBVS.World = s_Camera.GetWorldMatrix();
	s_CBVS.WVP = wvp.Transpose();

	g_Renderer->SetVertexBuffer(s_Resource.RoomMesh.VBFloor.Ptr(), sizeof(BasicVertex), 0U);
	g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
	g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));
	g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), 0U, D3DGraphic::ePixelShader);
	g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), &s_CBPS, sizeof(ConstantsBufferPS));

	/// Floor
	g_Renderer->SetShaderResource(s_Resource.FloorDiffuseTex.Reference(), 0U);
	g_Renderer->Draw(6U, 0U);

	/// Wall
	g_Renderer->SetShaderResource(s_Resource.WallDiffuseTex.Reference(), 1U);
	g_Renderer->Draw(18U, 0U);
}

void ApplicationStenciling::UpdateScene(float /*elapsedTime*/, float /*totalTime*/)
{
	float x = s_Radius * sinf(s_Phi) * cosf(s_Theta);
	float z = s_Radius * sinf(s_Phi) * sinf(s_Theta);
	float y = s_Radius * cosf(s_Phi);

	Vec3 eyePos(x, y, z);
	Vec3 lookAt(0.0f, 0.0f, 0.0f);
	s_Camera.SetViewParams(eyePos, lookAt);
}

void ApplicationStenciling::ResizeWindow(uint32_t width, uint32_t height)
{
	s_Camera.SetProjParams(DirectX::XM_PIDIV4, (float)width / height, 1.0f, 1000.0f);

	Base::ResizeWindow(width, height);
}

void ApplicationStenciling::MouseMove(WPARAM wParam, int x, int y)
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
		float dx = 0.01f * static_cast<float>(x - m_LastMousePos[0]);
		float dy = 0.01f * static_cast<float>(y - m_LastMousePos[1]);

		s_Radius += dx - dy;

		s_Radius = Math::Clamp(s_Radius, 3.0f, 50.0f);
	}

	m_LastMousePos[0] = x;
	m_LastMousePos[1] = y;
}