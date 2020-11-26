#if 0

#include "Colorful/Public/GfxEngine.h"
#include "AssetTool/AssetDatabase.h"

GfxPipelineState GfxModel::s_PipelineState;

struct UniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

void GfxModel::initPipelineState()
{
	m_UniformBuffer = g_GfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

	static bool8_t s_Inited = false;
	if (s_Inited)
	{
		return;
	}

	s_PipelineState.setMaterial("ModelBasic.mat");
	s_PipelineState.setUniformBuffer(eVertexShader, m_UniformBuffer, 0u);

	s_Inited = true;
}

void GfxModel::load(const std::string& modelName)
{
	AssetTool::AssetDatabase::instance().tryToLoadModel(modelName, *this, g_GfxEngine.get());

	initPipelineState();
}

void GfxModel::draw(const Camera& camera, const GfxViewport& viewport, bool8_t wireframe)
{
	if (!m_Valid)
	{
		return;
	}

	Matrix world = Matrix();
	Matrix view = camera.viewMatrix();
	Matrix proj = camera.projectionMatrix();

	UniformBufferVS uboVS
	{
		world,
		view,
		proj
	};
	m_UniformBuffer->update(&uboVS, sizeof(UniformBufferVS), 0u);

	GfxScissor scissor
	{
		0.0f,
		0.0f,
		viewport.z,
		viewport.w
	};
	s_PipelineState.setViewport(viewport);
	s_PipelineState.setScissor(scissor);
	s_PipelineState.setFrameBuffer(g_GfxEngine->backBuffer());
	s_PipelineState.Wireframe = wireframe;

	g_GfxEngine->bindGfxPipelineState(&s_PipelineState);

	GfxScopeGpuMarker(DrawModel, Color::randomColor());
	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		s_PipelineState.setVertexBuffer(m_Meshes[i].VertexBuffer);
		s_PipelineState.setIndexBuffer(m_Meshes[i].IndexBuffer);

		for (uint32_t j = 0u; j < m_Meshes[i].Material.Textures.size(); ++j)
		{
			if (m_Meshes[i].Material.Textures[j].Type == GfxTexture::eDiffuse)
			{
				auto &diffuseTexture = m_Textures[GfxTexture::eDiffuse][m_Meshes[i].Material.Textures[j].Index];
				s_PipelineState.setCombinedTextureSampler(eFragmentShader, diffuseTexture, GfxFactory::instance()->linearSampler(), 1u);
			}
		}

		g_GfxEngine->drawIndexed(m_Meshes[i].IndexCount, 1u, 0u, 0);
	}
}

void GfxModel::draw(GfxPipelineState* state)
{
	if (!m_Valid)
	{
		return;
	}

	g_GfxEngine->bindGfxPipelineState(state);

	GfxScopeGpuMarker(DrawModel, Color::randomColor());

	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		state->setVertexBuffer(m_Meshes[i].VertexBuffer);
		state->setIndexBuffer(m_Meshes[i].IndexBuffer);
		g_GfxEngine->drawIndexed(m_Meshes[i].IndexCount, 1u, 0u, 0);
	}
}
#endif

#include "Colorful/Public/GfxModel.h"

NAMESPACE_START(Gfx)

Model Model::createCube(float32_t length)
{
	ModelDesc desc;
	desc.setVertexUsageFlags(EVertexUsage::Position | EVertexUsage::Normal | EVertexUsage::Texcoord0);
	desc.SubMeshes.resize(1u);
	desc.IndexFormat = EIndexFormat::UInt16;
	desc.FaceCount = 6u;
	desc.SubMeshes[0u].FaceCount = 6u;

	float32_t halfLen = 0.5f * length;

	struct Vertex
	{
		Vec3 Position;
		Vec3 Normal;
		Vec2 UV;
	};
	std::vector<Vertex> vertices
	{
		{ { -halfLen, -halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ {  halfLen, -halfLen, -halfLen }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ { -halfLen, -halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ { -halfLen,  halfLen,  halfLen }, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
		{ { -halfLen,  halfLen, -halfLen }, { 0.0f, 1.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen,  halfLen }, { 0.0f, 1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 0.0f, 1.0f,  0.0f }, { 1.0f, 0.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 0.0f, 1.0f,  0.0f }, { 1.0f, 1.0f } },
		{ { -halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ {  halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ { -halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -halfLen, -halfLen,  halfLen }, {-1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -halfLen,  halfLen,  halfLen }, {-1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ { -halfLen,  halfLen, -halfLen }, {-1.0f, 0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ { -halfLen, -halfLen, -halfLen }, {-1.0f, 0.0f,  0.0f }, { 1.0f, 1.0f } },
		{ {  halfLen, -halfLen, -halfLen }, { 1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ {  halfLen,  halfLen, -halfLen }, { 1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  halfLen,  halfLen,  halfLen }, { 1.0f, 0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ {  halfLen, -halfLen,  halfLen }, { 1.0f, 0.0f,  0.0f }, { 1.0f, 1.0f } },
	};
	std::vector<uint16_t> indices
	{
		 0u,  1u,  2u,  0u,  2u,  3u, /// front
		 4u,  5u,  6u,  4u,  6u,  7u, /// back
		 8u,  9u, 10u,  8u, 10u, 11u, /// top
		12u, 13u, 14u, 12u, 14u, 15u, /// bottom
		16u, 17u, 18u, 16u, 18u, 19u, /// left
		20u, 21u, 22u, 20u, 22u, 23u  /// right
	};

	desc.setVertices(0u, vertices);
	desc.setIndices(0u, indices);

	return Model(desc);
}

NAMESPACE_END(Gfx)
