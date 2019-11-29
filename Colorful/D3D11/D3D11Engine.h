#pragma once

#include "D3D11Device.h"

class d3d11Engine : public rEngine, public singleton<d3d11Engine>
{
	singletonDeclare(d3d11Engine);
public:
	void initialize(uint64_t windowHandle, const appConfig &config) override final;
	void finalize() override final {}

	void logError(uint32_t result) const override final;

	inline void handleWindowResize(uint32_t, uint32_t, const appConfig &) override final
	{

	}

	inline rDevicePtr getDevice() const override final
	{
		assert(m_Device);
		return m_Device;
	}

	inline rContextPtr getIMContext() const override final
	{
		assert(m_IMContext);
		return m_IMContext;
	}
protected:
private:
	d3d11DevicePtr m_Device = nullptr;
	d3d11ContextPtr m_IMContext = nullptr;
};