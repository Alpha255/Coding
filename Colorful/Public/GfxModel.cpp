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
	vertexShader->setUniformBuffer(m_UniformBuffer, 0u);

	s_PipelineState.setShader(vertexShader);
	s_PipelineState.setShader(fragmentShader);

	GfxSamplerDesc desc;
	s_LinearSampler = g_GfxEngine->createSampler(desc);

	s_Inited = true;
}

void GfxModel::load(const std::string& modelName)
{
	AssetTool::AssetDatabase::instance().tryToLoadModel(modelName, *this, g_GfxEngine.get());

	initPipelineState();

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

void GfxModel::draw(const DXUTCamera& camera, const GfxViewport& viewport)
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

	g_GfxEngine->bindGfxPipelineState(&s_PipelineState);

	GfxScopeGpuMarker(DrawModel, Color::randomColor());

	for (uint32_t i = 0u; i < m_Meshes.size(); ++i)
	{
		s_PipelineState.setVertexBuffer(m_Meshes[i].VertexBuffer);
		s_PipelineState.setIndexBuffer(m_Meshes[i].IndexBuffer);
		g_GfxEngine->drawIndexed(m_Meshes[i].IndexCount, 0u, 0);
	}
}

void GfxModel::draw(const DXUTCamera&, GfxPipelineState&)
{
	if (!m_Valid)
	{
		return;
	}
}
