#pragma once

#if 0

#include "AssetTool/Asset.h"

class D3D11InputLayout : public D3DObject<ID3D11InputLayout>
{
public:
	D3D11InputLayout(
		const class D3D11Device& device, 
		const AssetTool::ShaderBinary& binary, 
		const std::vector<GfxVertexAttributes>& layout,
		uint32_t alignment);

	inline uint32_t stride() const
	{
		return m_Stride;
	}
protected:
private:
	uint32_t m_Stride = 0u;
};
using D3D11InputLayoutPtr = std::shared_ptr<D3D11InputLayout>;

#endif

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

DECLARE_SHARED_PTR(D3D11Shader)
class D3D11Shader : public D3DObject<ID3D11DeviceChild>, public Shader
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
class D3D11VertexShader : public D3DObject<ID3D11VertexShader>, public Shader
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
class D3D11HullShader : public D3DObject<ID3D11HullShader>, public Shader
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
class D3D11DomainShader : public D3DObject<ID3D11DomainShader>, public Shader
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
class D3D11GeometryShader : public D3DObject<ID3D11GeometryShader>, public Shader
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
class D3D11FragmentShader : public D3DObject<ID3D11PixelShader>, public Shader
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
class D3D11ComputeShader : public D3DObject<ID3D11ComputeShader>, public Shader
{
public:
	D3D11ComputeShader(ID3D11Device* device, const std::vector<uint32_t>& binary)
		: Shader(EShaderStage::Fragment)
	{
		assert(device && binary.size());
		VERIFY_D3D(device->CreateComputeShader(binary.data(), binary.size() * sizeof(uint32_t), nullptr, reference()));
	}
};

NAMESPACE_END(Gfx)