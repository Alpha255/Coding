#pragma once

#include "D3D11Device.h"
#include "D3D11Swapchain.h"

class d3d11Engine : public rEngine, public singleton<d3d11Engine>
{
	singletonDeclare(d3d11Engine);
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

	inline rDevice *getDevice() override final
	{
		assert(m_Device.isValid());
		return &m_Device;
	}
protected:
private:
	d3d11Device m_Device;
	d3d11Context m_IMContext;
	d3d11Swapchain m_Swapchain;
};