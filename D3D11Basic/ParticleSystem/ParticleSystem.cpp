#include "ParticleSystem.h"

#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct ConstantsBuffer
{
	Vec3 EmitPos = { 0.0f, 0.0f, 0.0f };
	float ElapseTime = 0.0f;

	Vec3 EmitDir = { 0.0f, 1.0f, 0.0f };
	float TotalTime = 0.0f;

	Vec4 EyePos = { 0.0f, 0.0f, 0.0f, 0.0f };

	Matrix VP;
};

static ConstantsBuffer s_CBuffer;

void ParticleSystem::Init()
{
	assert(g_Renderer);

	if (m_bInited)
	{
		return;
	}

	static const char *const s_TexNames[eTypeCount] =
	{
		"flare.dds",
		///"raindrop.dds"
	};

	static const char *const s_ShaderNames[eTypeCount] =
	{
		"ParticleFire.hlsl",
		///"ParticleRain.hlsl"
	};

	static uint32_t s_MaxParticleCount[eTypeCount] = 
	{
		500U, 
		///10000U
	};

	g_Renderer->CreateRandomTexture1D(m_Res.SRVRandomTex);

	g_Renderer->CreateConstantBuffer(m_Res.CBuf, sizeof(ConstantsBuffer), D3D11_USAGE_DYNAMIC,
		nullptr, D3D11_CPU_ACCESS_WRITE);

	static ParticleVertex v;
	g_Renderer->CreateVertexBuffer(m_Res.VBSrc, sizeof(ParticleVertex), D3D11_USAGE_DEFAULT, &v);

	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "Position",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Velocity",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Size",       0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "ElapseTime", 0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Type",       0, DXGI_FORMAT_R32_UINT,        0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	g_Renderer->CreateVertexShaderAndInputLayout(m_Res.VSStreamOut[eFire], m_Res.InputLayout, 
		layout, ARRAYSIZE(layout), s_ShaderNames[eFire], "VSStreamOut");
	///g_Renderer->CreateVertexShader(m_Res.VSStreamOut[eRain], s_ShaderNames[eRain], "VSStreamOut");

	for (uint32_t i = 0U; i < eTypeCount; ++i)
	{
		g_Renderer->CreateShaderResourceView(m_Res.SRVTextures[i], s_TexNames[i]);

		g_Renderer->CreateVertexShader(m_Res.VSDraw[i], s_ShaderNames[i], "VSDraw");

		g_Renderer->CreateGeometryShader(m_Res.GSStreamOut[i], s_ShaderNames[i], "GSStreamOut");
		g_Renderer->CreateGeometryShader(m_Res.GSDraw[i], s_ShaderNames[i], "GSDraw");

		g_Renderer->CreatePixelShader(m_Res.PixelShaders[i], s_ShaderNames[i], "PSMain");

		g_Renderer->CreateVertexBuffer(m_Res.VBDraw[i], sizeof(ParticleVertex) * s_MaxParticleCount[i],
			D3D11_USAGE_DEFAULT, nullptr, 0U, D3D11_BIND_STREAM_OUTPUT);
		g_Renderer->CreateVertexBuffer(m_Res.VBStreamOut[i], sizeof(ParticleVertex) * s_MaxParticleCount[i],
			D3D11_USAGE_DEFAULT, nullptr, 0U, D3D11_BIND_STREAM_OUTPUT);
	}

	m_bInited = true;
}

void ParticleSystem::Draw(const Camera &cam)
{
	static bool s_bFirstDraw = true;

	s_CBuffer.EyePos = cam.GetEyePos();
	if (eFire == m_Type)
	{
		s_CBuffer.EmitPos = { 0.0f, 1.0f, 120.0f };
	}
	else if (eRain == m_Type)
	{
		memcpy(&s_CBuffer.EmitPos, &s_CBuffer.EyePos, sizeof(Vec3));
	}
	Matrix vp = cam.GetViewMatrix() * cam.GetProjMatrix();
	s_CBuffer.VP = vp.Transpose();

	g_Renderer->UpdateBuffer(m_Res.CBuf, &s_CBuffer, sizeof(ConstantsBuffer));

	g_Renderer->SetInputLayout(m_Res.InputLayout);

	if (s_bFirstDraw)
	{
		g_Renderer->SetVertexBuffer(m_Res.VBSrc, sizeof(ParticleVertex), 0U);
	}
	else
	{
		g_Renderer->SetVertexBuffer(m_Res.VBDraw[m_Type], sizeof(ParticleVertex), 0U);
	}

	g_Renderer->SetStreamOut(m_Res.VBStreamOut[m_Type]);

	if (s_bFirstDraw)
	{
		g_Renderer->Draw(1U, 0U, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		s_bFirstDraw = false;
	}
	else
	{
		g_Renderer->DrawAuto();
	}

	static Ref<ID3D11Buffer> s_NullBuffer;
	g_Renderer->SetStreamOut(s_NullBuffer);

	std::swap(m_Res.VBDraw[m_Type], m_Res.VBStreamOut[m_Type]);

	g_Renderer->SetVertexBuffer(m_Res.VBDraw[m_Type], sizeof(ParticleVertex), 0U);

	g_Renderer->DrawAuto();
}

void ParticleSystem::Update(float elapseTime, float totalTime)
{
	s_CBuffer.ElapseTime = elapseTime;
	s_CBuffer.TotalTime = totalTime;
}
