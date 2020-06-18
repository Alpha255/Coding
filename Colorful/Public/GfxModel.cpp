#include "Colorful/Public/GfxEngine.h"
#include "AssetTool/AssetDatabase.h"
///#include <ThirdParty/assimp/include/assimp/material.h>

GfxPipelineState GfxModel::s_PipelineState;
GfxSamplerPtr GfxModel::s_LinearSampler = nullptr;

struct UniformBufferVS
{
	Matrix World;
	Matrix View;
	Matrix Proj;
};

void GfxModel::initPipelineState(GfxEngine* gfxEngine)
{
	assert(gfxEngine);

	m_UniformBuffer = gfxEngine->createUniformBuffer(sizeof(UniformBufferVS), nullptr);

	static bool8_t s_Inited = false;
	if (s_Inited)
	{
		return;
	}

	auto vertexShader = gfxEngine->createVertexShader("ModelBasic.shader");
	auto fragmentShader = gfxEngine->createFragmentShader("ModelBasic.shader");

	std::vector<GfxVertexAttributes> vertexAttrs
	{
		{
			ePosition,
			eRGB32_Float,
			eVertex
		},
		{
			eNormal,
			eRGB32_Float,
			eVertex
		},
		{
			eTangent,
			eRGB32_Float,
			eVertex
		},
		{
			eBiTangent,
			eRGB32_Float,
			eVertex
		},
		{
			eTexcoord,
			eRG32_Float,
			eVertex
		},
		{
			eColor,
			eRGBA32_Float,
			eVertex
		}
	};
	vertexShader->setInputLayout(vertexAttrs, alignof(GfxVertex));
	vertexShader->setUniformBuffer(m_UniformBuffer, 0u);

	s_PipelineState.setShader(vertexShader);
	s_PipelineState.setShader(fragmentShader);

	GfxSamplerDesc desc;
	s_LinearSampler = gfxEngine->createSampler(desc);

	s_Inited = true;
}

void GfxModel::load(const std::string& modelName, GfxEngine* gfxEngine)
{
	m_Name = File::name(modelName);

	AssetTool::AssetDatabase::instance().tryToLoadModel(modelName, *this, gfxEngine);

	initPipelineState(gfxEngine);

	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		for (uint32_t j = 0u; j < m_Meshes[i].Material.Textures.size(); ++j)
		{
			if (m_Meshes[i].Material.Textures[j].Type == 1u)
			{
				auto &diffuseTexture = m_Textures[1u][m_Meshes[i].Material.Textures[j].Index];
				s_PipelineState.Shaders[eFragmentShader]->setCombinedTextureSampler(diffuseTexture, s_LinearSampler, 1u);
			}
		}
	}
}

void GfxModel::draw(const DXUTCamera& camera, GfxEngine* gfxEngine, const GfxViewport& viewport)
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
	s_PipelineState.setFrameBuffer(gfxEngine->backBuffer());

	gfxEngine->bindGfxPipelineState(&s_PipelineState);

	GfxScopeGpuMarker("Model", Color::randomColor());

	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		s_PipelineState.bindVertexBuffer(m_Meshes[i].VertexBuffer);
		s_PipelineState.bindIndexBuffer(m_Meshes[i].IndexBuffer);
		gfxEngine->drawIndexed(m_Meshes[i].IndexCount, 0u, 0);
	}
}

void GfxModel::draw(const DXUTCamera&, GfxEngine*, GfxPipelineState&)
{
	if (!m_Valid)
	{
		return;
	}
}
