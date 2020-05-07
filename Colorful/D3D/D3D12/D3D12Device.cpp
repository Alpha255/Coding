#include "D3D12Device.h"
#include "D3D12Engine.h"

void d3d12CommandQueue::create(const d3d12Device &device)
{
	assert(!isValid() && device.isValid());

	D3D12_COMMAND_QUEUE_DESC desc
	{
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		0,
		D3D12_COMMAND_QUEUE_FLAG_NONE,
		0u
	};

	ID3D12CommandQueue *pCommandQueue = nullptr;
	rVerifyD3D12(device->CreateCommandQueue(&desc, _uuidof(ID3D12CommandQueue), (void **)&pCommandQueue));
	reset(pCommandQueue);
}

void d3d12Device::create(const dxgiFactory7 &inDxgiFactory)
{
	assert(!isValid() && inDxgiFactory.isValid());

	std::vector<dxgiAdapter4> dxgiAdapters;
	uint32_t adapterIndex = 0u;
	while (true)
	{
		IDXGIAdapter1 *pDxgiAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == (inDxgiFactory->EnumAdapters1(adapterIndex++, &pDxgiAdapter)))
		{
			break;
		}

		DXGI_ADAPTER_DESC1 adapterDesc = {};
		pDxgiAdapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		dxgiAdapters.emplace_back();
		dxgiAdapters[dxgiAdapters.size() - 1u].reset(static_cast<IDXGIAdapter4 *>(pDxgiAdapter));
	}

	struct createResult
	{
		D3D_FEATURE_LEVEL FeatureLevel = (D3D_FEATURE_LEVEL)0u;
		::HRESULT Result = E_FAIL;
	};

	auto tryToCreateDevice = [](const dxgiAdapter4 &adapter, d3d12Device &device, bool8_t realCreate)->createResult {
		createResult resultAttr = {};

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

		ID3D12Device *pDevice = nullptr;

		for (uint32_t j = 0; j < featureLevels.size(); ++j)
		{
			resultAttr.Result = D3D12CreateDevice(
				adapter.get(),
				featureLevels[j],
				_uuidof(ID3D12Device),
				realCreate ? (void **)&pDevice : nullptr);
			if (SUCCEEDED(resultAttr.Result))
			{
				resultAttr.FeatureLevel = featureLevels[j];

				if (realCreate)
				{
					device.reset(pDevice);
				}

				return resultAttr;
			}
		}

		return resultAttr;
	};

	adapterIndex = std::numeric_limits<uint32_t>().max();
	createResult tempResultAttr = {};
	for (uint32_t i = 0u; i < dxgiAdapters.size(); ++i)
	{
		d3d12Device tempDevice;

		createResult resultAttr = tryToCreateDevice(dxgiAdapters[i], tempDevice, false);
		if (SUCCEEDED(resultAttr.Result) && resultAttr.FeatureLevel > tempResultAttr.FeatureLevel)
		{
			tempResultAttr = resultAttr;
			adapterIndex = i;
		}
	}
	assert(adapterIndex != std::numeric_limits<uint32_t>().max());
	m_dxgiAdapter = dxgiAdapters[adapterIndex];

	createResult resultAttr = tryToCreateDevice(m_dxgiAdapter, *this, true);
	if (FAILED(resultAttr.Result))
	{
		Logger::instance().log(Logger::eError, "Failed to create d3d11 device.");
		d3d12Engine::instance().logError((uint32_t)resultAttr.Result);
		assert(0);
	}

	DXGI_ADAPTER_DESC3 adapterDesc = {};
	rVerifyD3D12(m_dxgiAdapter->GetDesc3(&adapterDesc));
	m_Adapter.VendorID = adapterDesc.VendorId;
	m_Adapter.DeviceID = adapterDesc.DeviceId;
	std::wstring wDeviceName(adapterDesc.Description);
	m_Adapter.DeviceName = std::string(wDeviceName.cbegin(), wDeviceName.cend());

	Logger::instance().log(Logger::eInfo, "Created d3d12 device on adapter: \"%s\", DeviceID = %d.",
		m_Adapter.DeviceName.c_str(),
		m_Adapter.DeviceID);

	m_CommandQueue.create(*this);
}