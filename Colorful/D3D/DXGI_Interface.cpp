#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

const char8_t* const D3DErrorLog::getErrorMessage(::HRESULT result)
{
	/// https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-returnvalues
	switch (result)
	{
	case D3D11_ERROR_FILE_NOT_FOUND: 
		return "The file was not found";
	case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: 
		return "There are too many unique instances of a particular type of state object";
	case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: 
		return "There are too many unique instances of a particular type of view object";
	case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
		return "The first call to ID3D11DeviceContext::Map after either "
			"ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource "
			"was not D3D11_MAP_WRITE_DISCARD";
	case DXGI_ERROR_INVALID_CALL: 
		return "The method call is invalid. For example, a method's parameter may not be a valid pointer";
	case DXGI_ERROR_WAS_STILL_DRAWING:
		return "The previous blit operation that is transferring information to or from this surface is incomplete";
	case E_FAIL:
		return "Attempted to create a device with the debug layer enabled and the layer is not installed";
	case E_INVALIDARG:
		return "An invalid parameter was passed to the returning function";
	case E_OUTOFMEMORY:
		return "Direct3D could not allocate sufficient memory to complete the call";
	case E_NOTIMPL:
		return "The method call isn't implemented with the passed parameter combination";
	case D3D12_ERROR_ADAPTER_NOT_FOUND:
		return "The specified cached PSO was created on a different adapter and cannot be reused on the current adapter.";
	case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
		return "The specified cached PSO was created on a different driver version and cannot be reused on the current adapter.";
	case S_FALSE:
		return "Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context)";
	case S_OK:
		return "Successed";
	}

	return "Unknown error";
}

DXGIFactory::DXGIFactory()
{
	uint32_t flags = 0u;
#if defined(_DEBUG)
	flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	VERIFY_D3D(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory2), reinterpret_cast<void**>(reference())));

	m_Factory0 = queryAs<DXGIFactory, DXGIFactory0>(*this);
	m_Factory1 = queryAs<DXGIFactory, DXGIFactory1>(*this);
	m_Factory3 = queryAs<DXGIFactory, DXGIFactory3>(*this);
	m_Factory4 = queryAs<DXGIFactory, DXGIFactory4>(*this);
	m_Factory5 = queryAs<DXGIFactory, DXGIFactory5>(*this);
	m_Factory6 = queryAs<DXGIFactory, DXGIFactory6>(*this);
	m_Factory7 = queryAs<DXGIFactory, DXGIFactory7>(*this);

	assert(m_Factory0.isValid() && m_Factory1.isValid());

	uint32_t subVersion = 2;
	if (m_Factory3.isValid())
	{
		subVersion = 3;
		if (m_Factory4.isValid())
		{
			subVersion = 4;
			if (m_Factory5.isValid())
			{
				subVersion = 5;
				if (m_Factory6.isValid())
				{
					subVersion = 6;
					if (m_Factory7.isValid())
					{
						subVersion = 7;
					}
				}
			}
		}
	}

	LOG_INFO("DXGI Factory Subversion = %d", subVersion);
}

NAMESPACE_END(Gfx)
