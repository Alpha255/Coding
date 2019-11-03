#include "D3D11Device.h"
#include "D3D11Engine.h"

void d3d11Device::create(__out d3d11ContextPtr &context, d3d11DxgiFactoryPtr &dxgiFactory)
{
	assert(!isValid() && !context->isValid() && dxgiFactory->isValid());

	std::vector<d3d11DxgiAdapterPtr> dxgiAdapters;
	uint32_t adapterIndex = 0u;
	while (true)
	{
		d3d11DxgiAdapterPtr dxgiAdapter = std::make_shared<d3d11DxgiAdapter>();
		IDXGIAdapter *pDxgiAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == ((*dxgiFactory)->EnumAdapters(adapterIndex++, &pDxgiAdapter)))
		{
			break;
		}
		dxgiAdapter->reset(pDxgiAdapter);
		dxgiAdapters.emplace_back(dxgiAdapter);
	}

	struct createResult
	{
		D3D_DRIVER_TYPE DriverType = (D3D_DRIVER_TYPE)0u;
		D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0u;
		::HRESULT Result = E_FAIL;
	};

	auto tryToCreateDevice = [](d3d11DevicePtr &device, d3d11ContextPtr &context)->createResult {
		createResult resultAttr = {};

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

		ID3D11Device *pDevice = nullptr;
		ID3D11DeviceContext *pContext = nullptr;

		for (uint32_t i = 0; i < driverTypes.size(); ++i)
		{
			resultAttr.Result = D3D11CreateDevice(
				nullptr,
				driverTypes[i],
				nullptr,
				deviceFlags,
				featureLevels.data(),
				(uint32_t)featureLevels.size(),
				D3D11_SDK_VERSION,
				&pDevice,
				&resultAttr.FeatureLevel,
				&pContext);
			if (SUCCEEDED(resultAttr.Result))
			{
				resultAttr.DriverType = driverTypes[i];

				device->reset(pDevice);
				context->reset(pContext);

				break;
			}
		}

		return resultAttr;
	};

	adapterIndex = UINT32_MAX;
	createResult tempResultAttr = {};
	for (uint32_t i = 0u; i < dxgiAdapters.size(); ++i)
	{
		d3d11DevicePtr tempDevice = std::make_shared<d3d11Device>();
		d3d11ContextPtr tempContext = std::make_shared<d3d11Context>();

		createResult resultAttr = tryToCreateDevice(tempDevice, tempContext);
		if (SUCCEEDED(resultAttr.Result) && resultAttr.FeatureLevel > tempResultAttr.FeatureLevel)
		{
			tempResultAttr = resultAttr;
			adapterIndex = i;
		}
	}
	assert(adapterIndex != UINT32_MAX);

	d3d11DevicePtr tempDevice = std::make_shared<d3d11Device>();
	createResult resultAttr = tryToCreateDevice(tempDevice, context);
	if (FAILED(resultAttr.Result))
	{
		gear::log(gear::eError, "Failed to create d3d11 device.");
		d3d11Engine::instance().logError((uint32_t)resultAttr.Result);
		assert(0);
	}

	*this = *tempDevice;

	DXGI_ADAPTER_DESC adapterDesc = {};
	rVerifyD3D11((*dxgiAdapters[adapterIndex])->GetDesc(&adapterDesc));
	m_Adapter.VendorID = adapterDesc.VendorId;
	m_Adapter.DeviceID = adapterDesc.DeviceId;
	std::wstring wDeviceName(adapterDesc.Description);
	m_Adapter.DeviceName = std::string(wDeviceName.cbegin(), wDeviceName.cend());

	gear::log(gear::eInfo, "Created d3d11 device on adapter: \"%s\", VerdonID = %d, DeviceID = %d.", 
		m_Adapter.DeviceName.c_str(),
		m_Adapter.VendorID,
		m_Adapter.DeviceID);
}