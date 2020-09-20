#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

#if 0

D3D11Device::D3D11Device(DXGI_Factory& factory)
{
	assert(factory.isValid());

	uint32_t flags = 0u;
#if defined(_DEBUG)
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGIAdapter1* testPtr = nullptr;

	std::vector<DXGI_Adapter> adapters;
	uint32_t adapterIndex = 0u;
	while (true)
	{
		IDXGIAdapter1* adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == (factory->EnumAdapters1(adapterIndex++, &adapter)))
		{
			break;
		}

		DXGI_ADAPTER_DESC1 adapterDesc{};
		adapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		adapters.emplace_back(DXGI_Adapter(adapter));
	}

	std::vector<D3D_FEATURE_LEVEL> featureLevels
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	auto tryToCreateDevice = [&](__inout D3D11DevicePtr& device, __inout D3D11ContextPtr& IMContext, __out D3D_DRIVER_TYPE& driverType, __out D3D_FEATURE_LEVEL& featureLevel)->::HRESULT
	{
		std::vector<D3D_DRIVER_TYPE> driverTypes
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};

		::HRESULT result;
		for (uint32_t i = 0; i < driverTypes.size(); ++i)
		{
			result = D3D11CreateDevice(
				nullptr,
				driverTypes[i],
				nullptr,
				flags,
				featureLevels.data(),
				(uint32_t)featureLevels.size(),
				D3D11_SDK_VERSION,
				device->reference(),
				&featureLevel,
				reinterpret_cast<ID3D11DeviceContext**>(IMContext->reference()));
			if (SUCCEEDED(result))
			{
				driverType = driverTypes[i];
				break;
			}
		}

		return result;
	};

	adapterIndex = std::numeric_limits<uint32_t>().max();
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_UNKNOWN;

	for (uint32_t i = 0u; i < adapters.size(); ++i)
	{
		D3D11DevicePtr tempDevice;
		D3D11ContextPtr tempIMContext;
		D3D_DRIVER_TYPE tempDriverType = D3D_DRIVER_TYPE_UNKNOWN;
		D3D_FEATURE_LEVEL tempFeatureLevel = D3D_FEATURE_LEVEL_1_0_CORE;

		if (SUCCEEDED(tryToCreateDevice(tempDevice, tempIMContext, tempDriverType, tempFeatureLevel)) && tempFeatureLevel > featureLevel)
		{
			adapterIndex = i;
			featureLevel = tempFeatureLevel;
			driverType = tempDriverType;
		}
	}
	assert(adapterIndex != std::numeric_limits<uint32_t>().max());

	VERIFY_D3D(D3D11CreateDevice(
		nullptr,
		driverType,
		nullptr,
		flags,
		featureLevels.data(),
		(uint32_t)featureLevels.size(),
		D3D11_SDK_VERSION,
		reference(),
		&featureLevel,
		reinterpret_cast<ID3D11DeviceContext**>(m_IMContext->reference())));

	m_Adapter = std::move(adapters[adapterIndex]);

	DXGI_ADAPTER_DESC3 adapterDesc{};
	VERIFY_D3D(m_Adapter->GetDesc3(&adapterDesc));
	VendorID = adapterDesc.VendorId;
	DeviceID = adapterDesc.DeviceId;
	std::wstring wDeviceName(adapterDesc.Description);
	DeviceName = std::string(wDeviceName.cbegin(), wDeviceName.cend());

	LOG_INFO("Created d3d11 device on adapter: \"%s\", DeviceID = %d.", DeviceName.c_str(), DeviceID);
}

#endif

NAMESPACE_END(Gfx)