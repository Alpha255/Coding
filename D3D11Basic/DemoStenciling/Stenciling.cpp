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
	Ref<ID3D11PixelShader> PS;

	Ref<ID3D11RasterizerState> NoBackFaceCulling;
	Ref<ID3D11SamplerState> Sampler;

	Ref<ID3D11BlendState> NoRTWrite;
	Ref<ID3D11DepthStencilState> MarkMirror;

	Ref<ID3D11RasterizerState> CloclwiseCulling;
	Ref<ID3D11DepthStencilState> Reflection;
	
	Ref<ID3D11BlendState> Transparent;
	Ref<ID3D11DepthStencilState> NoDoubleBlend;

	SimpleMesh SkullMesh;
	SceneRoomVertices RoomMesh;
};

struct ConstantsBufferVS
{
	Matrix World;
	Matrix WorldInverse;
	Matrix WVP;
};

struct ConstantsBufferPS
{
	Vec4 EyePos;
	Lighting::DirectionalLight DirLights[3];
	Lighting::Material Mat;

	uint32_t UseLighting = 1U;
	uint32_t UseTexture = 1U;
	uint32_t UseFog = 0U;
	uint32_t UseAlphaClip = 0U;

	Vec4 FogClr = Color::Black;
	
	float FogStart = 2.0f;
	float FogRange = 40.0f;
	Vec2 Padding;
};

static DemoStencilingResource s_Resource;
static ConstantsBufferVS s_CBVS;
static ConstantsBufferPS s_CBPS;
static float s_Radius = 12.0f;
static float s_Phi = DirectX::XM_PI * 0.42f;
static float s_Theta = DirectX::XM_PI * 1.24f;
static Camera s_Camera;
static Matrix s_SkullWorld;
static Matrix s_SkullWorldReflect;
static Lighting::DirectionalLight s_DirLights[3];
static Lighting::DirectionalLight s_DirLightsReflect[3];
static Lighting::Material s_MatRoom;
static Lighting::Material s_MatMirror;
static char* const s_ShaderName = "Stenciling.hlsl";

ApplicationStenciling::ApplicationStenciling()
{
	s_MatRoom.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_MatRoom.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	s_MatRoom.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);

	s_MatMirror.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_MatMirror.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 0.5f);
	s_MatMirror.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);

	s_DirLights[0].Ambient = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_DirLights[0].Diffuse = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_DirLights[0].Specular = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	s_DirLights[0].Direction = Vec4(0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_DirLights[1].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_DirLights[1].Diffuse = Vec4(0.20f, 0.20f, 0.20f, 1.0f);
	s_DirLights[1].Specular = Vec4(0.25f, 0.25f, 0.25f, 1.0f);
	s_DirLights[1].Direction = Vec4(-0.57735f, -0.57735f, 0.57735f, 0.0f);

	s_DirLights[2].Ambient = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_DirLights[2].Diffuse = Vec4(0.2f, 0.2f, 0.2f, 1.0f);
	s_DirLights[2].Specular = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	s_DirLights[2].Direction = Vec4(0.0f, -0.707f, -0.707f, 0.0f);

	memcpy(s_CBPS.DirLights, s_DirLights, sizeof(Lighting::DirectionalLight) * 3U);
	memcpy(s_DirLightsReflect, s_DirLights, sizeof(Lighting::DirectionalLight) * 3U);

	Matrix reflect = Matrix::Reflect(0.0f, 0.0f, 1.0f);
	for (uint32_t i = 0U; i < 3U; ++i)
	{
		s_DirLightsReflect[i].Direction.TransformNormal(reflect);
	}
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

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	g_Renderer->CreateVertexShaderAndInputLayout(s_Resource.VS.Reference(), s_Resource.RoomMesh.Layout.Reference(), layout, ARRAYSIZE(layout), s_ShaderName, "VS_Main");
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

	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	g_Renderer->CreateSamplerState(s_Resource.Sampler.Reference(), &sampDesc);

	g_Renderer->CreateRasterizerState(s_Resource.NoBackFaceCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_NONE);

	/// http://blog.csdn.net/qq_29523119/article/details/52824991
	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = 1 * (sr, sg, sb) + 0 * (dr, dg, db)
	/// outout(a) = 1 * sa + 0 * da
	D3D11_BLEND_DESC noRTWrite = { 0 };
	noRTWrite.AlphaToCoverageEnable = false;
	noRTWrite.IndependentBlendEnable = false;
	noRTWrite.RenderTarget[0].BlendEnable = false;
	noRTWrite.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRTWrite.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRTWrite.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRTWrite.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRTWrite.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRTWrite.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRTWrite.RenderTarget[0].RenderTargetWriteMask = 0U;
	g_Renderer->CreateBlendState(s_Resource.NoRTWrite.Reference(), &noRTWrite);

	/// http://blog.csdn.net/bonchoix/article/details/8499179
	D3D11_DEPTH_STENCIL_DESC markMirror = { 0 };
	markMirror.DepthEnable = true;
	markMirror.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	markMirror.DepthFunc = D3D11_COMPARISON_LESS;
	markMirror.StencilEnable = true;
	markMirror.StencilReadMask = 0xff;
	markMirror.StencilWriteMask = 0xff;
	markMirror.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	markMirror.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markMirror.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	markMirror.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	markMirror.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  	/// We are not rendering backfacing polygons, so these settings do not matter.
	markMirror.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	markMirror.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	markMirror.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	g_Renderer->CreateDepthStencilState(s_Resource.MarkMirror.Reference(), &markMirror);

	s_Resource.SkullMesh.CreateFromTxt("skull.txt");
	Lighting::Material skullMat;
	skullMat.Ambient = Vec4(0.5f, 0.5f, 0.5f, 1.0f);
	skullMat.Diffuse = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat.Specular = Vec4(0.4f, 0.4f, 0.4f, 16.0f);
	s_Resource.SkullMesh.SetMaterial(skullMat);
	s_Resource.SkullMesh.SetLightCount(3U);
	s_SkullWorld = Matrix::RotationAxis(0.0f, 1.0f, 0.0f, DirectX::XM_PIDIV2) * Matrix::Scaling(0.45f, 0.45f, 0.45f) * Matrix::Translation(0.0f, 1.0f, -5.0f);
	s_SkullWorldReflect = s_SkullWorld * Matrix::Reflect(0.0f, 0.0f, 1.0f);

	g_Renderer->CreateRasterizerState(s_Resource.CloclwiseCulling.Reference(), D3D11_FILL_SOLID, D3D11_CULL_BACK, true);

	D3D11_DEPTH_STENCIL_DESC reflection = { 0 };
	reflection.DepthEnable = true;
	reflection.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	reflection.DepthFunc = D3D11_COMPARISON_LESS;
	reflection.StencilEnable = true;
	reflection.StencilReadMask = 0xff;
	reflection.StencilWriteMask = 0xff;
	reflection.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	reflection.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflection.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	reflection.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	reflection.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  	/// We are not rendering backfacing polygons, so these settings do not matter.
	reflection.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	reflection.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	reflection.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	g_Renderer->CreateDepthStencilState(s_Resource.Reflection.Reference(), &reflection);

	/// src(sr, sg, sb, sa)  dst(dr, dg, db, da)
	/// output(r, g, b) = sa * (sr, sg, sb) + (1 - sa) * (dr, dg, db)
	/// output(a) = 1 * sa + 0 * da
	D3D11_BLEND_DESC transparent = { 0 };
	transparent.AlphaToCoverageEnable = false;
	transparent.IndependentBlendEnable = false;
	transparent.RenderTarget[0].BlendEnable = true;
	transparent.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparent.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparent.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparent.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparent.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparent.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparent.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_Renderer->CreateBlendState(s_Resource.Transparent.Reference(), &transparent);

	D3D11_DEPTH_STENCIL_DESC noDoubleBlend = { 0 };
	noDoubleBlend.DepthEnable = true;
	noDoubleBlend.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlend.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlend.StencilEnable = true;
	noDoubleBlend.StencilReadMask = 0xff;
	noDoubleBlend.StencilWriteMask = 0xff;
	noDoubleBlend.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlend.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlend.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlend.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlend.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;  	/// We are not rendering backfacing polygons, so these settings do not matter.
	noDoubleBlend.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlend.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlend.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

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

	Matrix wvp = s_Camera.GetWorldMatrix() * s_Camera.GetViewMatrix() * s_Camera.GetProjMatrix();
	s_CBVS.World = s_Camera.GetWorldMatrix();
	s_CBVS.World = s_CBVS.World.Transpose();
	s_CBVS.WorldInverse = s_CBVS.World.Inverse();
	s_CBVS.WVP = wvp.Transpose();

	s_CBPS.EyePos = s_Camera.GetEyePos();

	///g_Renderer->SetRasterizerState(s_Resource.NoBackFaceCulling.Ptr());

	/// Draw floor and walls

	{
		/// Floor
		g_Renderer->SetVertexShader(s_Resource.VS.Ptr());
		g_Renderer->SetPixelShader(s_Resource.PS.Ptr());
		g_Renderer->SetInputLayout(s_Resource.RoomMesh.Layout.Ptr());
		g_Renderer->SetVertexBuffer(s_Resource.RoomMesh.VBFloor.Ptr(), sizeof(BasicVertex), 0U);
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));
		memcpy(&s_CBPS.Mat, &s_MatRoom, sizeof(Lighting::Material));
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), 0U, D3DGraphic::ePixelShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), &s_CBPS, sizeof(ConstantsBufferPS));
		g_Renderer->SetShaderResource(s_Resource.FloorDiffuseTex.Reference(), 0U);
		g_Renderer->SetSamplerStates(s_Resource.Sampler.Reference());
		g_Renderer->Draw(6U, 0U);
	}

	{
		/// Wall
		g_Renderer->SetVertexShader(s_Resource.VS.Ptr());
		g_Renderer->SetPixelShader(s_Resource.PS.Ptr());
		g_Renderer->SetInputLayout(s_Resource.RoomMesh.Layout.Ptr());
		g_Renderer->SetVertexBuffer(s_Resource.RoomMesh.VBWall.Ptr(), sizeof(BasicVertex), 0U);
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));
		memcpy(&s_CBPS.Mat, &s_MatRoom, sizeof(Lighting::Material));
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), 0U, D3DGraphic::ePixelShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), &s_CBPS, sizeof(ConstantsBufferPS));
		g_Renderer->SetShaderResource(s_Resource.WallDiffuseTex.Reference(), 0U);
		g_Renderer->SetSamplerStates(s_Resource.Sampler.Reference());
		g_Renderer->Draw(18U, 0U);
	}

	{
		/// Draw Skull
		for (uint32_t i = 0U; i < 3U; ++i)
		{
			s_Resource.SkullMesh.SetLight(i, s_DirLights[i]);
		}
		s_Resource.SkullMesh.SetWorldMatrix(s_SkullWorld);
		s_Resource.SkullMesh.Draw(s_Camera);
	}

	{
		/// dst = MaterialSkull, src = MaterialMirror
		/// Draw mirror to stencil buffer
		g_Renderer->SetVertexShader(s_Resource.VS.Ptr());
		g_Renderer->SetPixelShader(nullptr);
		g_Renderer->SetInputLayout(s_Resource.RoomMesh.Layout.Ptr());
		g_Renderer->SetVertexBuffer(s_Resource.RoomMesh.VBMirror.Ptr(), sizeof(BasicVertex), 0U);
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));

		/// Do not write to render target.
		g_Renderer->SetBlendState(s_Resource.NoRTWrite.Ptr(), Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);

		/// Render visible mirror pixels to stencil buffer.
		/// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
		g_Renderer->SetDepthStencilState(s_Resource.MarkMirror.Ptr(), 1U);
		g_Renderer->Draw(6U, 0U);

		/// Restore states.
		g_Renderer->SetDepthStencilState(nullptr, 0U);
		g_Renderer->SetBlendState(nullptr, Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}

	/// Draw skull reflection
	{
		for (uint32_t i = 0U; i < 3U; ++i)
		{
			s_Resource.SkullMesh.SetLight(i, s_DirLightsReflect[i]);
		}
		s_Resource.SkullMesh.SetWorldMatrix(s_SkullWorldReflect);
		g_Renderer->SetRasterizerState(s_Resource.CloclwiseCulling.Ptr());
		g_Renderer->SetDepthStencilState(s_Resource.Reflection.Ptr(), 1U);
		s_Resource.SkullMesh.Draw(s_Camera);

		g_Renderer->SetRasterizerState(nullptr);
		g_Renderer->SetDepthStencilState(nullptr, 0U);
	}

	/// Draw the mirror to the back buffer as usual but with transparency blending so the reflection shows through.
	{
		/// dst = MaterialSkull, src = MaterialMirror
		g_Renderer->SetVertexShader(s_Resource.VS.Ptr());
		g_Renderer->SetPixelShader(s_Resource.PS.Ptr());
		g_Renderer->SetInputLayout(s_Resource.RoomMesh.Layout.Ptr());
		g_Renderer->SetVertexBuffer(s_Resource.RoomMesh.VBMirror.Ptr(), sizeof(BasicVertex), 0U);
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), 0U, D3DGraphic::eVertexShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBVS.Ptr(), &s_CBVS, sizeof(ConstantsBufferVS));
		memcpy(&s_CBPS.Mat, &s_MatMirror, sizeof(Lighting::Material));
		g_Renderer->SetConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), 0U, D3DGraphic::ePixelShader);
		g_Renderer->UpdateConstantBuffer(s_Resource.RoomMesh.CBPS.Ptr(), &s_CBPS, sizeof(ConstantsBufferPS));
		g_Renderer->SetShaderResource(s_Resource.MirrorDiffuseTex.Reference());
		g_Renderer->SetBlendState(s_Resource.Transparent.Ptr(), Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		g_Renderer->Draw(6U, 0U);
	}

	/// Draw the skull shadow.
	{
		//Vec4 shadowPlane = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
		//Vec4 toLight = s_DirLights[0].Direction * -1.0f;
		//Matrix skullShadowWorld = Matrix::Shadow(shadowPlane, toLight) * Matrix::Translation(0.0f, 0.001f, 0.0f);
		//s_Resource.SkullMesh.SetWorldMatrix(skullShadowWorld);
		//g_Renderer->SetDepthStencilState(s_Resource.NoDoubleBlend.Ptr(), 0U);
		//s_Resource.SkullMesh.Draw(s_Camera);
		//g_Renderer->SetBlendState(nullptr, Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		//g_Renderer->SetDepthStencilState(nullptr, 0U);
	}
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