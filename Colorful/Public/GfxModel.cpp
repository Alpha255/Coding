#include "Colorful/Public/GfxEngine.h"
#include "AssetTool/AssetDatabase.h"

GfxPipelineState GfxModel::s_PipelineState;
GfxPipelineState GfxModel::s_PipelineState_BoundingBox;
GfxSamplerPtr GfxModel::s_LinearSampler = nullptr;

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

	auto vertexShader = g_GfxEngine->createVertexShader("ModelBasic.shader");
	auto fragmentShader = g_GfxEngine->createFragmentShader("ModelBasic.shader");

	std::vector<GfxVertexAttributes> vertexAttrs
	{
		{
			ePosition,
			eRGB32_Float
		},
		{
			eNormal,
			eRGB32_Float
		},
		{
			eTangent,
			eRGB32_Float
		},
		{
			eBiTangent,
			eRGB32_Float
		},
		{
			eTexcoord,
			eRG32_Float
		},
		{
			eColor,
			eRGBA32_Float
		}
	};
	vertexShader->setInputLayout(vertexAttrs, alignof(GfxVertex));

	s_PipelineState.setUniformBuffer(eVertexShader, m_UniformBuffer, 0u);
	s_PipelineState.setShader(vertexShader);
	s_PipelineState.setShader(fragmentShader);

	GfxSamplerDesc desc;
	s_LinearSampler = g_GfxEngine->createSampler(desc);

	vertexShader = g_GfxEngine->createVertexShader("ModelBasic_BoundingBox.shader");
	fragmentShader = g_GfxEngine->createFragmentShader("ModelBasic_BoundingBox.shader");
	vertexAttrs.clear();
	GfxVertexAttributes attr
	{
		ePosition,
		eRGB32_Float
	};
	vertexAttrs.emplace_back(std::move(attr));
	vertexShader->setInputLayout(vertexAttrs, alignof(Vec3));
	s_PipelineState_BoundingBox.setShader(vertexShader);
	s_PipelineState_BoundingBox.setShader(fragmentShader);
	s_PipelineState_BoundingBox.setUniformBuffer(eVertexShader, m_UniformBuffer, 0u);
	GfxRasterizerStateDesc rasterizerState
	{
		eWireframe
	};
	s_PipelineState_BoundingBox.setRasterizerState(rasterizerState);
	s_PipelineState_BoundingBox.setPrimitiveTopology(eTriangleStrip);

	auto vertices = m_BoundingBox.vertices();
	m_BoundingBoxVertexCount = (uint32_t)vertices.size();
	s_PipelineState_BoundingBox.VertexBuffer = g_GfxEngine->createVertexBuffer(eGpuReadWrite, sizeof(Vec3) * vertices.size(), vertices.data());

	s_Inited = true;
}

void GfxModel::load(const std::string& modelName)
{
	AssetTool::AssetDatabase::instance().tryToLoadModel(modelName, *this, g_GfxEngine.get());

	initPipelineState();
}

void GfxModel::focusCamera(DXUTCamera& camera)
{
	auto lookAt = m_BoundingBox.center();
	auto eye = lookAt + Vec3(0.0f, 0.0f, -5.0f);
	camera.setView(eye, lookAt);
}

void GfxModel::draw(const DXUTCamera& camera, const GfxViewport& viewport, bool8_t wireframe, bool8_t boundingBox)
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
				s_PipelineState.setCombinedTextureSampler(eFragmentShader, diffuseTexture, s_LinearSampler, 1u);
			}
		}

		g_GfxEngine->drawIndexed(m_Meshes[i].IndexCount, 1u, 0u, 0);
	}

	if (boundingBox)
	{
		s_PipelineState_BoundingBox.setViewport(viewport);
		s_PipelineState_BoundingBox.setScissor(scissor);
		s_PipelineState_BoundingBox.setFrameBuffer(g_GfxEngine->backBuffer());
		g_GfxEngine->bindGfxPipelineState(&s_PipelineState_BoundingBox);
		g_GfxEngine->draw(m_BoundingBoxVertexCount, 1u, 0u);
	}
}

void GfxModel::draw(const GfxPipelineState* state)
{
	if (!m_Valid)
	{
		return;
	}

	g_GfxEngine->bindGfxPipelineState(state);

	GfxScopeGpuMarker(DrawModel, Color::randomColor());

	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		s_PipelineState.setVertexBuffer(m_Meshes[i].VertexBuffer);
		s_PipelineState.setIndexBuffer(m_Meshes[i].IndexBuffer);
		g_GfxEngine->drawIndexed(m_Meshes[i].IndexCount, 1u, 0u, 0);
	}
}
