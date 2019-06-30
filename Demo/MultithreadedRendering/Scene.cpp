#include "Scene.h"

const float Scene::s_SceneRadius = 600.0f;

Matrix Scene::ConstantsBufferPS::CalcLightMatrix(uint32_t iLight, float fov, float aspect, float nearPlane, float farPlane)
{
	const Vec4 &LightDir = LightParams[iLight].Direction;
	const Vec4 &LightPos = LightParams[iLight].Position;
	const Vec3 Up = Vec3(0.0f, 1.0f, 0.0f);

	Vec4 lookAt = LightPos + LightDir * Scene::s_SceneRadius;

	Matrix view = Matrix::LookAtLH(Vec3(LightPos.x, LightPos.y, LightPos.z), Vec3(lookAt.x, lookAt.y, lookAt.z), Up);
	Matrix proj = Matrix::PerspectiveFovLH(fov, aspect, nearPlane, farPlane);

	return view * proj;
}

Scene::ConstantsBufferPS::ConstantsBufferPS()
{
	const Vec4 SceneCenter = Vec4(0.0f, 350.0f, 0.0f, 1.0f);

	LightParams[0].Color = Vec4(0.8f, 0.8f, 0.8f, 1.0f);
	LightParams[0].Direction = Vec4(-0.67f, -0.71f, +0.21f, 0.0);
	LightParams[0].Position = SceneCenter - LightParams[0].Direction * Scene::s_SceneRadius;

	LightParams[1].Color = Vec4(0.4f * 0.895f, 0.4f * 0.634f, 0.4f * 0.626f, 1.0f);
	LightParams[1].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
	LightParams[1].Position = Vec4(0.0f, 400.0f, -250.0f, 1.0f);

	LightParams[2].Color = Vec4(0.5f * 0.388f, 0.5f * 0.641f, 0.5f * 0.401f, 1.0f);
	LightParams[2].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
	LightParams[2].Position = Vec4(0.0f, 400.0f, 0.0f, 1.0f);

	LightParams[3].Color = Vec4(0.4f * 1.000f, 0.4f * 0.837f, 0.4f * 0.848f, 1.0f);
	LightParams[3].Direction = Vec4(0.00f, -1.00f, 0.00f, 0.0f);
	LightParams[3].Position = Vec4(0.0f, 400.0f, 250.0f, 1.0f);
}

void Scene::Shadow::Initialize()
{
	const uint32_t ShadowMapSize = 2048U;

	RTexture2D shadowMap;
	shadowMap.Create(eR32_Typeless, ShadowMapSize, ShadowMapSize, eBindAsShaderResource | eBindAsDepthStencil, 1U);

	ShadowMap.CreateAsTexture(eTexture2D, shadowMap, eR32_Float, 1U, 1U);
	DepthMap.CreateAsTexture(eTexture2D, shadowMap, eD32_Float, 0U, 0U);

	Params.TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	Params.MirrorPlane = Vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

void Scene::Mirror::Initialize(RVertexShader &vertexShader)
{
	/// These values are hard-coded based on the sdkmesh contents, plus some
	/// hand-fiddling, pending a better solution in the pipeline.
	Vec3 MirrorCenters[Scene::eMirrorCount];
	MirrorCenters[0] = Vec3(-35.1688f, 89.279683f, -0.7488765f);
	MirrorCenters[1] = Vec3(41.2174f, 89.279683f, -0.7488765f);
	MirrorCenters[2] = Vec3(3.024275f, 89.279683f, -54.344299f);
	MirrorCenters[3] = Vec3(3.024275f, 89.279683f, 52.8466f);

	Vec2 MirrorSize[Scene::eMirrorCount];
	MirrorSize[0] = Vec2(104.190895f, 92.19922656f);
	MirrorSize[1] = Vec2(104.190899f, 92.19922656f);
	MirrorSize[2] = Vec2(76.3862f, 92.3427325f);
	MirrorSize[3] = Vec2(76.386196f, 92.34274043f);

	Vec3 MirrorNormals[Scene::eMirrorCount];
	MirrorNormals[0] = Vec3(-0.998638464f, -0.052165297f, 0.0f);
	MirrorNormals[1] = Vec3(0.998638407f, -0.052166381f, 3.15017E-08f);
	MirrorNormals[2] = Vec3(0.0f, -0.076278878f, -0.997086522f);
	MirrorNormals[3] = Vec3(-5.22129E-08f, -0.076279957f, 0.99708644f);

	Vec2 MirrorResolution[Scene::eMirrorCount];
	MirrorResolution[0].x = MirrorResolution[1].x = MirrorResolution[2].x = MirrorResolution[3].x = 320.0f;
	MirrorResolution[0].y = (MirrorResolution[0].x * MirrorSize[0].y / MirrorSize[0].x);
	MirrorResolution[1].y = (MirrorResolution[1].x * MirrorSize[1].y / MirrorSize[1].x);
	MirrorResolution[2].y = (MirrorResolution[2].x * MirrorSize[2].y / MirrorSize[2].x);
	MirrorResolution[3].y = (MirrorResolution[3].x * MirrorSize[3].y / MirrorSize[3].x);

	Vec3 MirrorCorners[Scene::eMirrorCount];
	MirrorCorners[0] = Vec3(-1.0f, -1.0f, 0.0f);
	MirrorCorners[1] = Vec3(1.0f, -1.0f, 0.0f);
	MirrorCorners[2] = Vec3(-1.0f, 1.0f, 0.0f);
	MirrorCorners[3] = Vec3(1.0f, 1.0f, 0.0f);

	Vec3 MirrorPointAt[Scene::eMirrorCount];
	const Vec3 Up(0.0f, 1.0f, 0.0f);

	VertexBuffer.CreateAsVertexBuffer(sizeof(MirrorRect), eGpuReadCpuWrite, nullptr);

	for (uint32_t i = 0U; i < Scene::eMirrorCount; ++i)
	{
		Params[i].TintColor = Vec4(0.3f, 0.5f, 1.0f, 1.0f);
		Params[i].MirrorPlane = Math::GetPlaneFromPointNormal(MirrorCenters[i], MirrorNormals[i]);

		MirrorPointAt[i] = MirrorNormals[i] + MirrorCenters[i];
		World[i] = Matrix::Transpose(Matrix::LookAtLH(MirrorPointAt[i], MirrorCenters[i], Up));
		World[i]._41 = MirrorCenters[i].x;
		World[i]._42 = MirrorCenters[i].y;
		World[i]._43 = MirrorCenters[i].z;
		World[i]._14 = World[i]._24 = World[i]._34 = 0.0f;
		World[i]._44 = 1.0f;

		for (uint32_t j = 0U; j < Scene::eMirrorCount; ++j)
		{
			Vertices[i][j].Position = Vec3(0.5f * MirrorSize[i].x * MirrorCorners[j].x, 0.5f * MirrorSize[i].y * MirrorCorners[j].y, MirrorCorners[j].z);
			Vertices[i][j].Normal = Vec3(0.0f, 0.0f, 0.0f);
			Vertices[i][j].Tangent = Vec3(0.0f, 0.0f, 0.0f);
			Vertices[i][j].UV = Vec2(0.0f, 0.0f);
		}
	}

	std::vector<Geometry::VertexLayout> vertexLayout =
	{
		{ "POSITION", sizeof(Geometry::Vertex::Position), offsetof(Geometry::Vertex, Position), eRGB32_Float  },
		{ "NORMAL",   sizeof(Geometry::Vertex::Normal),   offsetof(Geometry::Vertex, Normal),   eRGB32_Float  },
		{ "TANGENT",  sizeof(Geometry::Vertex::Tangent),  offsetof(Geometry::Vertex, Tangent),  eRGB32_Float  },
		{ "TEXCOORD", sizeof(Geometry::Vertex::UV),       offsetof(Geometry::Vertex, UV),       eRG32_Float   },
	};
	Layout.Create(vertexShader.GetBlob(), vertexLayout);
}

void Scene::Initialize()
{
	const float FOV[4] =
	{
		Math::XM_PI / 4.0f ,
		65.0f * (Math::XM_PI / 180.0f),
		65.0f * (Math::XM_PI / 180.0f),
		65.0f * (Math::XM_PI / 180.0f)
	};

	for (uint32_t i = 0U; i < eLightCount; ++i)
	{
		CBuffer_VS.LightVP[i] = CBuffer_PS.Memory.CalcLightMatrix(i, FOV[i], 1.0f, 100.0f, 2.0f * s_SceneRadius);
		CBuffer_PS.Memory.LightParams[i].Falloff = Vec4(2.0f * s_SceneRadius, 100.0f, cosf(FOV[i] / 2.0f), 0.1f);
	}

	VertexShader.Create("MultithreadedRendering.shader", "VSMain");
	PixelShader.Create("MultithreadedRendering.shader", "PSMain");

	SquidRoom.CreateFromFile("SquidRoom.sdkmesh");

	CBuffer_VS.Buffer.CreateAsUniformBuffer(sizeof(ConstantsBufferVS), eGpuReadCpuWrite);
	CBuffer_PS.Buffer.CreateAsUniformBuffer(sizeof(ConstantsBufferPS), eGpuReadCpuWrite);

	NoStencil.Create(
		true, eDepthMaskAll, eRComparisonFunc::eLessEqual,
		false, 0U, 0U,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eNever,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eNever);

	DepthTestStencilOverwrite.Create(
		true, eDepthMaskZero, eRComparisonFunc::eLessEqual,
		true, 0U, eStencilDefaultWriteMask,
		eRStencilOp::eReplace, eRStencilOp::eKeep, eRStencilOp::eReplace, eRComparisonFunc::eAlways,
		eRStencilOp::eReplace, eRStencilOp::eKeep, eRStencilOp::eReplace, eRComparisonFunc::eAlways);

	DepthOverwriteStencilTest.Create(
		true, eDepthMaskAll, eRComparisonFunc::eAlways,
		true, eStencilDefaultReadMask, 0U,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual);

	DepthWriteStencilTest.Create(
		true, eDepthMaskAll, eRComparisonFunc::eLessEqual, /// eAlways
		true, eStencilDefaultReadMask, 0U,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual,
		eRStencilOp::eKeep, eRStencilOp::eKeep, eRStencilOp::eKeep, eRComparisonFunc::eEqual);

	DepthOverwriteStencilClear.Create(
		true, eDepthMaskAll, eRComparisonFunc::eAlways,
		true, eStencilDefaultReadMask, eStencilDefaultWriteMask,
		eRStencilOp::eZero, eRStencilOp::eKeep, eRStencilOp::eZero, eRComparisonFunc::eEqual,
		eRStencilOp::eZero, eRStencilOp::eKeep, eRStencilOp::eZero, eRComparisonFunc::eEqual);

	Params.TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	ShadowRes.Initialize();
	MirrorRes.Initialize(VertexShader);
}

void Scene::SetMirror(const DXUTCamera &camera, uint32_t index)
{
	RPixelShader NullPixelShader;
	REngine::Instance().SetRasterizerState(RStaticState::SolidFrontCCW);

	REngine::Instance().SetInputLayout(MirrorRes.Layout);
	REngine::Instance().SetVertexBuffer(MirrorRes.VertexBuffer, sizeof(Geometry::Vertex), 0U, 0U);
	REngine::Instance().SetVertexShader(VertexShader);
	REngine::Instance().SetPixelShader(NullPixelShader);
	REngine::Instance().SetUniformBuffer(CBuffer_VS.Buffer, 0U, eVertexShader);

	CBuffer_VS.Memory.World = Matrix::Transpose(MirrorRes.World[index]);
	CBuffer_VS.Memory.WorldInverse = Matrix::InverseTranspose(MirrorRes.World[index]);
	CBuffer_VS.Memory.VP = Matrix::Transpose(camera.GetViewMatrix() * camera.GetProjMatrix());
	CBuffer_VS.Buffer.Update(&CBuffer_VS.Memory, sizeof(ConstantsBufferVS));
}

void Scene::DrawMirror(const DXUTCamera &camera, uint32_t index)
{
	/// Test for back-facing mirror (from whichever pov we are using)
	REvent Marker;

	Vec3 eyePos = camera.GetEyePos();
	if (Math::PlaneDotCoord(MirrorRes.Params[index].MirrorPlane, Vec4(eyePos.x, eyePos.y, eyePos.z, 0.0f)) < 0.0f)
	{
		return;
	}

	/// Draw mirror rect
	MirrorRes.VertexBuffer.Update(MirrorRes.Vertices[index], sizeof(Mirror::MirrorRect));

	Marker.Begin("Darw Mirror Rect-DepthTestStencilOverwrite");
	SetMirror(camera, index);
	REngine::Instance().SetDepthStencilState(DepthTestStencilOverwrite, 0x01U);
	REngine::Instance().Draw(4U, 0U, eTriangleStrip);
	Marker.End();

	Marker.Begin("Darw Mirror Rect-DepthOverwriteStencilTest");
	Matrix vp = camera.GetViewMatrix() * camera.GetProjMatrix();
	CBuffer_VS.Memory.VP._31 = vp._14;
	CBuffer_VS.Memory.VP._32 = vp._24;
	CBuffer_VS.Memory.VP._33 = vp._34;
	CBuffer_VS.Memory.VP._34 = vp._44;
	CBuffer_VS.Buffer.Update(&CBuffer_VS.Memory, sizeof(ConstantsBufferVS));
	REngine::Instance().SetDepthStencilState(DepthOverwriteStencilTest, 0x01U);
	REngine::Instance().Draw(4U, 0U, eTriangleStrip);
	Marker.End();

	/// Draw scene in reflection
	Marker.Begin("Darw Scene In Reflection");
	Matrix reflection = Matrix::Reflect(MirrorRes.Params[index].MirrorPlane);
	REngine::Instance().SetDepthStencilState(DepthWriteStencilTest, 0x01U);
	REngine::Instance().SetRasterizerState(RStaticState::SolidFrontCCW);
	DrawScene(camera, MirrorRes.Params[index], Matrix::IdentityMatrix(), reflection * vp);
	Marker.End();

	Marker.Begin("Darw Mirror Rect-DepthOverwriteStencilClear");
	SetMirror(camera, index);
	REngine::Instance().SetDepthStencilState(DepthOverwriteStencilClear, 0x01U);
	REngine::Instance().Draw(4U, 0U, eTriangleStrip);
	Marker.End();
}

void Scene::DrawShadow()
{
}

void Scene::DrawScene(const DXUTCamera &camera, const StaticParams &params, const Matrix &world, const Matrix &vp)
{
	REngine::Instance().SetVertexShader(VertexShader);
	REngine::Instance().SetSamplerState(RStaticState::LinearSampler, 0U, ePixelShader);
	REngine::Instance().SetSamplerState(RStaticState::PointClampSampler, 1U, ePixelShader);
	REngine::Instance().SetUniformBuffer(CBuffer_VS.Buffer, 0U, eVertexShader);

	CBuffer_VS.Memory.World = Matrix::Transpose(world);
	CBuffer_VS.Memory.WorldInverse = Matrix::InverseTranspose(world);
	CBuffer_VS.Memory.VP = Matrix::Transpose(vp);
	CBuffer_VS.Buffer.Update(&CBuffer_VS.Memory, sizeof(ConstantsBufferVS));

	REngine::Instance().SetPixelShader(PixelShader);

	REngine::Instance().SetUniformBuffer(CBuffer_PS.Buffer, 0U, ePixelShader);
	for (uint32_t i = 0U; i < eLightCount; ++i)
	{
		CBuffer_PS.Memory.LightParams[i].VP = Matrix::Transpose(CBuffer_VS.LightVP[i]);
	}
	CBuffer_PS.Memory.MirrorPlane = params.MirrorPlane;
	CBuffer_PS.Memory.TintColor = params.TintColor;
	CBuffer_PS.Buffer.Update(&CBuffer_PS.Memory, sizeof(ConstantsBufferPS));

	SquidRoom.Draw(camera, false);
}

void Scene::Draw(const DXUTCamera &camera, uint32_t width, uint32_t height)
{
	REvent Marker;

	REngine::Instance().ResetDefaultRenderSurfaces();
	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)width, (float)height));

	Marker.Begin("Draw Shadow");
	DrawShadow();
	Marker.End();

	Marker.Begin("Draw Mirrors");
	for (uint32_t i = 0U; i < eMirrorCount; ++i)
	{
		Marker.Begin(Base::FormatString("Draw Mirror: %d", i));
		DrawMirror(camera, i);
		Marker.End();
	}
	Marker.End();

	Marker.Begin("Draw Scene Normally");
	REngine::Instance().SetRasterizerState(RStaticState::Solid);
	REngine::Instance().SetDepthStencilState(NoStencil, 0U);
	DrawScene(camera, Params, Matrix::IdentityMatrix(), camera.GetWVPMatrix());
	Marker.End();
}

void Scene::Update(float elapsedTime, float totalTime)
{
	Vec4 Down(0.0f, -1.0f, 0.0f, 0.0f);

	float fCycle1X = 0.0f;
	float fCycle1Z = 0.20f * sinf(2.0f * (totalTime + 0.0f * Math::XM_PI));
	CBuffer_PS.Memory.LightParams[1].Direction = Down + Vec4(fCycle1X, 0.0f, fCycle1Z, 0.0f);

	float fCycle2X = 0.10f * cosf(1.6f * (totalTime + 0.3f * Math::XM_PI));
	float fCycle2Z = 0.10f * sinf(1.6f * (totalTime + 0.0f * Math::XM_PI));
	CBuffer_PS.Memory.LightParams[2].Direction = Down + Vec4(fCycle2X, 0.0f, fCycle2Z, 0.0f);

	float fCycle3X = 0.30f * cosf(2.4f * (totalTime + 0.3f * Math::XM_PI));
	float fCycle3Z = 0.0f;
	CBuffer_PS.Memory.LightParams[3].Direction = Down + Vec4(fCycle3X, 0.0f, fCycle3Z, 0.0f);
}
