#pragma once

#include "D3D12Device.h"
#include "D3D12Swapchain.h"

class d3d12Engine : public rEngine, public singleton<d3d12Engine>
{
	singletonDeclare(d3d12Engine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final;

	void logError(uint32_t result) const override final;

	inline void handleWindowResize(uint32_t width, uint32_t height, const appConfig &) override final
	{
		if (m_Swapchain.isValid())
		{
			m_Swapchain.resize(width, height);
		}
	}

	inline const d3d12Device &getDevice() const
	{
		assert(m_Device.isValid());
		return m_Device;
	}
protected:
private:
	d3d12Device m_Device;
	d3d12Swapchain m_Swapchain;
};