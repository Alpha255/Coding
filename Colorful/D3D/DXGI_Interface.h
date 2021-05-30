#pragma once

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "Colorful/Gfx/GfxRenderer.h"

NAMESPACE_START(Gfx)
//class DXGIOutput  final : public D3DObject<IDXGIOutput>  {};
//class DXGIOutput1 final : public D3DObject<IDXGIOutput1> {};
//class DXGIOutput2 final : public D3DObject<IDXGIOutput2> {};
//class DXGIOutput3 final : public D3DObject<IDXGIOutput3> {};
//class DXGIOutput4 final : public D3DObject<IDXGIOutput4> {};
//class DXGIOutput5 final : public D3DObject<IDXGIOutput5> {};
//class DXGIOutput6 final : public D3DObject<IDXGIOutput6> {};

template<class T>
class D3DObject : public GfxObject<T>
{
public:
	D3DObject() = default;

	D3DObject(Type* handle)
		: GfxObject<T>(handle)
	{
	}

	D3DObject(const D3DObject& other)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}
			if (other.m_Handle)
			{
				other.m_Handle->AddRef();
			}

			m_Handle = other.m_Handle;
		}
	}

	D3DObject(D3DObject&& other)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(other.m_Handle, {});
		}
	}

	D3DObject& operator=(const D3DObject& other)
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}
			if (other.m_Handle)
			{
				other.m_Handle->AddRef();
			}

			m_Handle = other.m_Handle;
		}
		return *this;
	}

	D3DObject& operator=(D3DObject&& other) noexcept
	{
		if (m_Handle != other.m_Handle)
		{
			if (m_Handle)
			{
				m_Handle->Release();
			}

			m_Handle = std::exchange(other.m_Handle, {});
		}
		return *this;
	}

	virtual ~D3DObject()
	{
		if (m_Handle)
		{
			m_Handle->Release();
			m_Handle = nullptr;
		}
	}
protected:
private:
};

class D3DErrorLog
{
public:
	static const char8_t* const getErrorMessage(::HRESULT result);
};

#define VERIFY_D3D(Func)                                                                \
{                                                                                       \
	::HRESULT result = (Func);                                                          \
	if (FAILED(result))                                                                 \
	{                                                                                   \
		LOG_ERROR("Failed to invoke D3DAPI, %s", D3DErrorLog::getErrorMessage(result)); \
	}                                                                                   \
}

template<class SrcInterface, class DstInterface>
DstInterface queryAs(SrcInterface& srcInterface)
{
	assert(srcInterface.isValid());

	DstInterface dstInterface;
	srcInterface->QueryInterface(__uuidof(DstInterface::Type), reinterpret_cast<void**>(dstInterface.reference()));
	return dstInterface;
}

class DXGIFactory0 final : public D3DObject<IDXGIFactory>  {};
class DXGIFactory1 final : public D3DObject<IDXGIFactory1> {};
class DXGIFactory3 final : public D3DObject<IDXGIFactory3> {};
class DXGIFactory4 final : public D3DObject<IDXGIFactory4> {};
class DXGIFactory5 final : public D3DObject<IDXGIFactory5> {};
class DXGIFactory6 final : public D3DObject<IDXGIFactory6> {};
class DXGIFactory7 final : public D3DObject<IDXGIFactory7> {};

/// https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_3/nf-dxgi1_3-createdxgifactory2
class DXGIFactory final : public D3DObject<IDXGIFactory2>
{
public:
	using D3DObject::D3DObject;

	DXGIFactory();

	IDXGIFactory* get0()
	{
		assert(m_Factory0.isValid());
		return m_Factory0.get();
	}

	IDXGIFactory1* get1()
	{
		assert(m_Factory1.isValid());
		return m_Factory1.get();
	}

	IDXGIFactory3* get3()
	{
		assert(m_Factory3.isValid());
		return m_Factory3.get();
	}

	IDXGIFactory4* get4()
	{
		assert(m_Factory4.isValid());
		return m_Factory4.get();
	}

	IDXGIFactory5* get5()
	{
		assert(m_Factory5.isValid());
		return m_Factory5.get();
	}

	IDXGIFactory6* get6()
	{
		assert(m_Factory6.isValid());
		return m_Factory6.get();
	}

	IDXGIFactory7* get7()
	{
		assert(m_Factory7.isValid());
		return m_Factory7.get();
	}
protected:
private:
	DXGIFactory0 m_Factory0;
	DXGIFactory1 m_Factory1;
	DXGIFactory3 m_Factory3;
	DXGIFactory4 m_Factory4;
	DXGIFactory5 m_Factory5;
	DXGIFactory6 m_Factory6;
	DXGIFactory7 m_Factory7;
};
DECLARE_UNIQUE_PTR(DXGIFactory)

class DXGIAdapter  final : public D3DObject<IDXGIAdapter>  {};
class DXGIAdapter1 final : public D3DObject<IDXGIAdapter1> {};
class DXGIAdapter2 final : public D3DObject<IDXGIAdapter2> {};
class DXGIAdapter3 final : public D3DObject<IDXGIAdapter3> {};
class DXGIAdapter4 final : public D3DObject<IDXGIAdapter4> {};

class DXGIAdapterList
{
public:
	DXGIAdapterList(IDXGIFactory2* factory2, IDXGIFactory6* factory6);

	IDXGIAdapter1* get1()
	{
		return m_Adapter1s.size() > 0u ? m_Adapter1s[0].get() : nullptr;
	}

	IDXGIAdapter4* get4()
	{
		return m_Adapter4s.size() > 0u ? m_Adapter4s[0].get() : nullptr;
	}
protected:
private:
	std::vector<DXGIAdapter1> m_Adapter1s;
	std::vector<DXGIAdapter4> m_Adapter4s;
};
DECLARE_UNIQUE_PTR(DXGIAdapterList)

class DXGISwapChain0 final : public D3DObject<IDXGISwapChain>  {};
class DXGISwapChain1 final : public D3DObject<IDXGISwapChain1> {};
class DXGISwapChain2 final : public D3DObject<IDXGISwapChain2> {};
class DXGISwapChain3 final : public D3DObject<IDXGISwapChain3> {};
class DXGISwapChain4 final : public D3DObject<IDXGISwapChain4> {};

class D3DShaderBlob final : public D3DObject<ID3DBlob> {};
class D3D11ShaderReflection final : public D3DObject<ID3D11ShaderReflection> {};

NAMESPACE_END(Gfx)
