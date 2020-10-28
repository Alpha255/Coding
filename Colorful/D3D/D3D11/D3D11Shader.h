#pragma once

#include "Colorful/D3D/D3D11/D3D11Map.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(D3D11Shader)
class D3D11Shader final : public D3DObject<ID3D11DeviceChild>, public Shader
{
public:
	D3D11Shader(ID3D11Device* device, EShaderStage stage, const std::vector<uint32_t>& binary)
		: Shader(stage)
	{
		assert(device && stage < EShaderStage::ShaderStageCount && binary.size());

		const void* byteCode = binary.data();
		size_t byteCodeLength = sizeof(uint32_t) * binary.size();
		ID3D11DeviceChild** handle = reference();

		switch (stage)
		{
		case EShaderStage::Vertex:
			VERIFY_D3D(device->CreateVertexShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11VertexShader**>(handle)));
			break;
		case EShaderStage::Hull:
			VERIFY_D3D(device->CreateHullShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11HullShader**>(handle)));
			break;
		case EShaderStage::Domain:
			VERIFY_D3D(device->CreateDomainShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11DomainShader**>(handle)));
			break;
		case EShaderStage::Geometry:
			VERIFY_D3D(device->CreateGeometryShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11GeometryShader**>(handle)));
			break;
		case EShaderStage::Fragment:
			VERIFY_D3D(device->CreatePixelShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11PixelShader**>(handle)));
			break;
		case EShaderStage::Compute:
			VERIFY_D3D(device->CreateComputeShader(byteCode, byteCodeLength, nullptr, reinterpret_cast<ID3D11ComputeShader**>(handle)));
			break;
		}
	}
protected:
private:
};

DECLARE_SHARED_PTR(D3D11VertexShader)
class D3D11VertexShader final : public D3DObject<ID3D11VertexShader>, public Shader
{
public:
	D3D11VertexShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Vertex)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateVertexShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11HullShader)
class D3D11HullShader final : public D3DObject<ID3D11HullShader>, public Shader
{
public:
	D3D11HullShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Hull)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateHullShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11DomainShader)
class D3D11DomainShader final : public D3DObject<ID3D11DomainShader>, public Shader
{
public:
	D3D11DomainShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Domain)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateDomainShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11GeometryShader)
class D3D11GeometryShader final : public D3DObject<ID3D11GeometryShader>, public Shader
{
public:
	D3D11GeometryShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Geometry)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateGeometryShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11FragmentShader)
class D3D11FragmentShader final : public D3DObject<ID3D11PixelShader>, public Shader
{
public:
	D3D11FragmentShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Fragment)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreatePixelShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11ComputeShader)
class D3D11ComputeShader final : public D3DObject<ID3D11ComputeShader>, public Shader
{
public:
	D3D11ComputeShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Fragment)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateComputeShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

DECLARE_SHARED_PTR(D3D11InputLayout)
class D3D11InputLayout final : public D3DObject<ID3D11InputLayout>, public InputLayout
{
public:
	D3D11InputLayout(ID3D11Device* device, const std::vector<VertexInputDesc>& descs, const std::vector<uint32_t>& binary, bool8_t isHLSL)
	{
		assert(device);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputDescs{};
		for (uint32_t i = 0u; i < descs.size(); ++i)
		{
			for (uint32_t j = 0u; j < descs[i].Layouts.size(); ++j)
			{
				auto& layout = descs[i].Layouts[j];
				D3D11_INPUT_ELEMENT_DESC inputDesc
				{
					isHLSL ? layout.Usage.c_str() : "TEXCOORD",
					isHLSL ? 0u : j,
					D3D11Map::format(layout.Format),
					i,
					D3D11_APPEND_ALIGNED_ELEMENT,  /// auto stride
					descs[i].InputRate == VertexInputDesc::EVertexInputRate::Instance ? 
						D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
					0u  /// ??? 
				};
				inputDescs.emplace_back(inputDesc);
			}
		}

		VERIFY_D3D(device->CreateInputLayout(
			inputDescs.data(),
			static_cast<uint32_t>(inputDescs.size()),
			reinterpret_cast<const void*>(binary.data()), 
			binary.size() * sizeof(uint32_t), 
			reference()));
	}
protected:
private:
};

NAMESPACE_END(Gfx)