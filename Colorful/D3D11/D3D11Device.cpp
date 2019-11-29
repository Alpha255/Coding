#include "D3D11Device.h"
#include "D3D11Engine.h"

void d3d11Device::create(__out d3d11ContextPtr &context, dxgiFactory7Ptr &dxgiFactoryPtr)
{
	assert(!isValid() && !context->isValid() && dxgiFactoryPtr && dxgiFactoryPtr->isValid());

	std::vector<dxgiAdapter4Ptr> dxgiAdapterPtrs;
	uint32_t adapterIndex = 0u;
	while (true)
	{
		dxgiAdapter4Ptr dxgiAdapterPtr = std::make_shared<dxgiAdapter4>();
		IDXGIAdapter1 *pDxgiAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == ((*dxgiFactoryPtr)->EnumAdapters1(adapterIndex++, &pDxgiAdapter)))
		{
			break;
		}
		dxgiAdapterPtr->reset(static_cast<IDXGIAdapter4 *>(pDxgiAdapter));
		dxgiAdapterPtrs.emplace_back(dxgiAdapterPtr);
	}

	struct createResult
	{
		D3D_DRIVER_TYPE DriverType = (D3D_DRIVER_TYPE)0u;
		D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0u;
		::HRESULT Result = E_FAIL;
	};

	auto tryToCreateDevice = [](d3d11DevicePtr &devicePtr, d3d11ContextPtr &contextPtr)->createResult {
		createResult resultAttr = {};

		uint32_t deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		///deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
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

				devicePtr->reset(pDevice);
				contextPtr->reset(pContext);

				break;
			}
		}

		return resultAttr;
	};

	adapterIndex = UINT32_MAX;
	createResult tempResultAttr = {};
	for (uint32_t i = 0u; i < dxgiAdapterPtrs.size(); ++i)
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
		logger::instance().log(logger::eError, "Failed to create d3d11 device.");
		d3d11Engine::instance().logError((uint32_t)resultAttr.Result);
		assert(0);
	}

	*this = *tempDevice;
	m_dxgiAdapter = dxgiAdapterPtrs[adapterIndex];

	DXGI_ADAPTER_DESC3 adapterDesc = {};
	rVerifyD3D11((*m_dxgiAdapter)->GetDesc3(&adapterDesc));
	m_Adapter.VendorID = adapterDesc.VendorId;
	m_Adapter.DeviceID = adapterDesc.DeviceId;
	std::wstring wDeviceName(adapterDesc.Description);
	m_Adapter.DeviceName = std::string(wDeviceName.cbegin(), wDeviceName.cend());

	logger::instance().log(logger::eInfo, "Created d3d11 device on adapter: \"%s\", DeviceID = %d.",
		m_Adapter.DeviceName.c_str(),
		m_Adapter.DeviceID);
}