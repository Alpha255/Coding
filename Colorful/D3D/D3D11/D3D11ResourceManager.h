#pragma once

#include "Colorful/Public/GfxResourceManager.h"
#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11RenderState.h"
#include "Colorful/D3D/D3D11/D3D11View.h"
#include "Colorful/D3D/D3D11/D3D11Device.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(D3D11ResourceManager)
class D3D11ResourceManager final : public GfxResourceManager
{
public:
	D3D11ResourceManager(D3D11DevicePtr device)
		: m_Device(device)
	{
		assert(device);
	}

	ShaderPtr createShader(const ShaderDesc& desc) override final
	{
		assert(desc.Stage < EShaderStage::ShaderStageCount);

		switch (desc.Stage)
		{
		case EShaderStage::Vertex:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11VertexShader>(m_Device->get(), desc.Binary));
		case EShaderStage::Hull:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11HullShader>(m_Device->get(), desc.Binary));
		case EShaderStage::Domain:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11DomainShader>(m_Device->get(), desc.Binary));
		case EShaderStage::Geometry:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11GeometryShader>(m_Device->get(), desc.Binary));
		case EShaderStage::Fragment:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11FragmentShader>(m_Device->get(), desc.Binary));
		case EShaderStage::Compute:
			return std::static_pointer_cast<Shader>(std::make_shared<D3D11ComputeShader>(m_Device->get(), desc.Binary));
		}

		return nullptr;
	}

	TexturePtr createTexture(const TextureDesc& desc) override final
	{
		assert(desc.Dimension < ETextureType::Buffer);

		switch (desc.Dimension)
		{
		case ETextureType::T_1D:
		case ETextureType::T_1D_Array:
			return std::static_pointer_cast<Texture>(std::make_shared<D3D11Texture1D>(m_Device->get(), desc));
		case ETextureType::T_2D:
		case ETextureType::T_2D_Array:
		case ETextureType::T_Cube:
		case ETextureType::T_Cube_Array:
			return std::static_pointer_cast<Texture>(std::make_shared<D3D11Texture2D>(m_Device->get(), desc));
		case ETextureType::T_3D:
			return std::static_pointer_cast<Texture>(std::make_shared<D3D11Texture3D>(m_Device->get(), desc));
		default:
			assert(0);
			return nullptr;
		}
	}

	SamplerPtr createSampler(const SamplerDesc& desc) override final
	{
		return std::static_pointer_cast<Sampler>(std::make_shared<D3D11Sampler>(m_Device->get(), desc));
	}

	InputLayoutPtr createInputLayout(const std::vector<VertexInputDesc>& descs, const std::vector<uint32_t>& binary) override final
	{
		return std::static_pointer_cast<InputLayout>(std::make_shared<D3D11InputLayout>(
			m_Device->get(),
			descs,
			binary,
			false));
	}

	GPUBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<D3D11Buffer>(
			m_Device->get(),
			EBindFlags::Bind_UniformBuffer,
			EBufferUsage::Dynamic,
			size,
			data));
	}

	GPUBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<D3D11Buffer>(
			m_Device->get(),
			EBindFlags::Bind_IndexBuffer,
			usage,
			size,
			data));
	}

	GPUBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return std::static_pointer_cast<GPUBuffer>(std::make_shared<D3D11Buffer>(
			m_Device->get(),
			EBindFlags::Bind_VertexBuffer,
			usage,
			size,
			data));
	}
protected:
	D3D11DevicePtr m_Device = nullptr;
};

extern D3D11ResourceManagerPtr GD3D11ResourceMgr;

NAMESPACE_END(Gfx)
