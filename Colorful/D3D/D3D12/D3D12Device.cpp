#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(Gfx)

D3D12Debug::D3D12Debug()
{
	VERIFY_D3D(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(reference())));

#if defined(_DEBUG)
	(*this)->EnableDebugLayer();
#endif
}

D3D12Device::D3D12Device(IDXGIAdapter1* adapter1, IDXGIAdapter4* adapter4)
{
	assert(adapter1);

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

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_1_0_CORE;
	for (auto level : featureLevels)
	{
		if (SUCCEEDED(D3D12CreateDevice(adapter4 ? adapter4 : adapter1,
			level,
			_uuidof(ID3D12Device),
			reinterpret_cast<void**>(reference()))))
		{
			featureLevel = level;
			break;
		}
	}
	assert(isValid());

	uint32_t VendorID = ~0u;
	uint32_t DeviceID = ~0u;
	std::wstring DeviceName;

	if (adapter4)
	{
		DXGI_ADAPTER_DESC3 adapterDesc;
		VERIFY_D3D(adapter4->GetDesc3(&adapterDesc));
		VendorID = adapterDesc.VendorId;
		DeviceID = adapterDesc.DeviceId;
		DeviceName.assign(adapterDesc.Description);
	}
	else
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		VERIFY_D3D(adapter1->GetDesc1(&adapterDesc));
		VendorID = adapterDesc.VendorId;
		DeviceID = adapterDesc.DeviceId;
		DeviceName.assign(adapterDesc.Description);
	}

	auto getFeatureLevelName = [](D3D_FEATURE_LEVEL level)->const char8_t* const
	{
		switch (level)
		{
		case D3D_FEATURE_LEVEL_9_1:  return "9.1";
		case D3D_FEATURE_LEVEL_9_2:  return "9.2";
		case D3D_FEATURE_LEVEL_9_3:  return "9.3";
		case D3D_FEATURE_LEVEL_10_0: return "10.0";
		case D3D_FEATURE_LEVEL_10_1: return "10.1";
		case D3D_FEATURE_LEVEL_11_0: return "11.0";
		case D3D_FEATURE_LEVEL_11_1: return "11.1";
		case D3D_FEATURE_LEVEL_12_0: return "12.0";
		case D3D_FEATURE_LEVEL_12_1: return "12.1";
		}
		return "Unknown";
	};

	LOG_INFO("Created D3D12 device on adapter: \"%s\", DeviceID = %d. Feature Level = %s", DeviceName.c_str(), DeviceID, getFeatureLevelName(featureLevel));
};

NAMESPACE_END(Gfx)