#pragma once

#include "Colorful/Gfx/GfxRenderState.h"

NAMESPACE_START(Gfx)

enum class EClearFlags : uint8_t
{
	Depth = 1 << 1,
	Stencil = 1 << 2,
	DepthStencil = Depth | Stencil
};

class CommandBuffer
{
public:
	virtual void beginDebugMarker(const char8_t* name, const Vec4& color) = 0;
	virtual void endDebugMarker() = 0;
	virtual void insertDebugMarker(const char8_t* name, const Vec4& color) = 0;
	virtual void setObjectDebugName(uint64_t objectHandle, EDescriptorType type, const char8_t* name) = 0;

	template<class LAMBDA>
	void executeRenderPass(const RenderPassDesc& renderPassDesc, GraphicsPipelineState& pipelineState, LAMBDA&& Lambda);

	///virtual void clearRenderTarget(TexturePtr renderTarget, const Math::Color& color) = 0;
	///virtual void clearDepthStencil(TexturePtr depthStencil, EClearFlags flags, float32_t depth, uint8_t stencil) = 0;

	///virtual void draw(uint32_t vertexCount, uint32_t firstVertex) = 0;
	///virtual void drawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
	virtual void drawIndexed(uint32_t indexCount, uint32_t firstIndex, int32_t vertexOffset) = 0;
	///virtual void drawIndexedInstanced(uint32_t vertexCount, uint32_t instanceCount, int32_t vertexOffset, uint32_t firstInstance) = 0;
	///virtual void drawIndirect(GpuBufferPtr indirectBuffer, uint32_t offset, uint32_t drawCount) = 0;
	///virtual void drawIndexedIndirect(GpuBufferPtr indirectBuffer, uint32_t offset, uint32_t drawCount) = 0;

	///virtual void dispatch(uint32_t threadGroupX, uint32_t threadGroupY, uint32_t threadGroupZ) = 0;

	virtual void drawModel(const class Model& model) = 0;
protected:
	virtual void beginRenderPass(const RenderPassDesc& renderPassDesc, GraphicsPipelineState& pipelineState) = 0;
	virtual void endRenderPass() = 0;
};

class GfxProfilerScope
{
public:
	GfxProfilerScope(CommandBuffer* cmdBuffer, const char8_t* name, const Vec4& color)
		: m_CmdBuffer(cmdBuffer)
	{
		assert(m_CmdBuffer && name);
		m_CmdBuffer->beginDebugMarker(name, color);
	}
	~GfxProfilerScope()
	{
		assert(m_CmdBuffer);
		m_CmdBuffer->endDebugMarker();
	}
protected:
private:
	CommandBuffer* m_CmdBuffer;
};

#define GFX_JUNCTION(A, B) A##B
#define GFX_PROFILER_SCOPE(CmdBuffer, Name, Color) GfxProfilerScope __GfxProfilerLocal(CmdBuffer, Name, Color)

template<class LAMBDA>
void CommandBuffer::executeRenderPass(const RenderPassDesc& renderPassDesc, GraphicsPipelineState& pipelineState, LAMBDA&& Lambda)
{
	GFX_PROFILER_SCOPE(this, renderPassDesc.Name.c_str(), Color::random());

	beginRenderPass(renderPassDesc, pipelineState);

	Lambda();

	endRenderPass();
}

NAMESPACE_END(Gfx)
