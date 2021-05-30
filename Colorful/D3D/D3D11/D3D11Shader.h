#pragma once

#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

class D3D11VertexShader final : public D3DObject<ID3D11VertexShader>, public Shader
{
public:
	D3D11VertexShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Vertex, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateVertexShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11VertexShader)

class D3D11HullShader final : public D3DObject<ID3D11HullShader>, public Shader
{
public:
	D3D11HullShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Hull, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateHullShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11HullShader)

class D3D11DomainShader final : public D3DObject<ID3D11DomainShader>, public Shader
{
public:
	D3D11DomainShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Domain, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateDomainShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11DomainShader)

class D3D11GeometryShader final : public D3DObject<ID3D11GeometryShader>, public Shader
{
public:
	D3D11GeometryShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Geometry, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateGeometryShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11GeometryShader)

class D3D11FragmentShader final : public D3DObject<ID3D11PixelShader>, public Shader
{
public:
	D3D11FragmentShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Fragment, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreatePixelShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11FragmentShader)

class D3D11ComputeShader final : public D3DObject<ID3D11ComputeShader>, public Shader
{
public:
	D3D11ComputeShader(ID3D11Device* device, ShaderDesc& desc)
		: Shader(EShaderStage::Fragment, desc.Reflections)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateComputeShader(desc.Binary.data(), desc.BinarySize, nullptr, reference()));
	}
};
DECLARE_SHARED_PTR(D3D11ComputeShader)

class D3D11InputLayout final : public D3DObject<ID3D11InputLayout>, public InputLayout
{
public:
	D3D11InputLayout(ID3D11Device* device, const VertexInputLayoutDesc& desc, const ShaderDesc& shaderDesc)
		: InputLayout(desc.PrimitiveTopology)
	{
		assert(device && shaderDesc.BinarySize);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		for (uint32_t i = 0u; i < desc.VertexInputs.size(); ++i)
		{
			for (uint32_t j = 0u; j < desc.VertexInputs[i].Layouts.size(); ++j)
			{
				assert(desc.VertexInputs[i].Binding < ELimitations::MaxVertexStreams);

				auto& layout = desc.VertexInputs[i].Layouts[j];
				D3D11_INPUT_ELEMENT_DESC inputElement
				{
					shaderDesc.Language == EShaderLanguage::HLSL ? layout.Usage.c_str() : "TEXCOORD",
					shaderDesc.Language == EShaderLanguage::HLSL ? 0u : j,
					D3D11EnumTranslator::format(layout.Format),
					desc.VertexInputs[i].Binding,
					D3D11_APPEND_ALIGNED_ELEMENT,  /// auto stride
					desc.VertexInputs[i].InputRate == VertexInputLayoutDesc::EVertexInputRate::Instance ?
						D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
					0u  /// ??? 
				};
				inputElements.emplace_back(std::move(inputElement));
			}
		}

		VERIFY_D3D(device->CreateInputLayout(
			inputElements.data(),
			static_cast<uint32_t>(inputElements.size()),
			reinterpret_cast<const void*>(shaderDesc.Binary.data()),
			shaderDesc.BinarySize,
			reference()));
	}
};
DECLARE_SHARED_PTR(D3D11InputLayout)

NAMESPACE_END(Gfx)