#include "D3D11Device.h"
#include "D3D11Engine.h"

void D3D11Device::create(__out D3D11Context& IMContext, const DXGIFactory7& factory)
{
	assert(!isValid() && !IMContext.isValid() && factory.isValid());

	std::vector<DXGIAdapter4> adapters;
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

		adapters.emplace_back();
		adapters[adapters.size() - 1u].reset(static_cast<IDXGIAdapter4*>(adapter));
	}

	struct CreateResult
	{
		D3D_DRIVER_TYPE DriverType = (D3D_DRIVER_TYPE)0u;
		D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0u;
		::HRESULT Result = E_FAIL;
		ID3D11Device* Device = nullptr;
		ID3D11DeviceContext* IMContext = nullptr;
	};

	auto tryToCreateDevice = []()->CreateResult {
		CreateResult result{};

		uint32_t deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		std::vector<D3D_DRIVER_TYPE> driverTypes =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};

		std::vector<D3D_FEATURE_LEVEL> featureLevels =
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

		for (uint32_t i = 0; i < driverTypes.size(); ++i)
		{
			result.Result = D3D11CreateDevice(
				nullptr,
				driverTypes[i],
				nullptr,
				deviceFlags,
				featureLevels.data(),
				(uint32_t)featureLevels.size(),
				D3D11_SDK_VERSION,
				&result.Device,
				&result.FeatureLevel,
				&result.IMContext);
			if (SUCCEEDED(result.Result))
			{
				result.DriverType = driverTypes[i];

				break;
			}
		}

		return result;
	};

	adapterIndex = std::numeric_limits<uint32_t>().max();
	CreateResult createResult{};
	for (uint32_t i = 0u; i < adapters.size(); ++i)
	{
		CreateResult tempResult = tryToCreateDevice();
		if (SUCCEEDED(tempResult.Result) && tempResult.FeatureLevel > createResult.FeatureLevel)
		{
			createResult = tempResult;
			adapterIndex = i;
		}
	}
	assert(adapterIndex != std::numeric_limits<uint32_t>().max());

	createResult = tryToCreateDevice();
	if (FAILED(createResult.Result))
	{
		Logger::instance().log(Logger::eError, "Failed to create d3d11 device.");
		D3D11Engine::instance().logError((uint32_t)createResult.Result);
		assert(0);
	}

	reset(createResult.Device);
	IMContext.reset(createResult.IMContext);
	m_DXGIAdapter = adapters[adapterIndex];

	DXGI_ADAPTER_DESC3 adapterDesc{};
	GfxVerifyD3D(m_DXGIAdapter->GetDesc3(&adapterDesc));
	m_Adapter.VendorID = adapterDesc.VendorId;
	m_Adapter.DeviceID = adapterDesc.DeviceId;
	std::wstring wDeviceName(adapterDesc.Description);
	m_Adapter.DeviceName = std::string(wDeviceName.cbegin(), wDeviceName.cend());

	Logger::instance().log(Logger::eInfo, "Created d3d11 device on adapter: \"%s\", DeviceID = %d.",
		m_Adapter.DeviceName.c_str(),
		m_Adapter.DeviceID);
}