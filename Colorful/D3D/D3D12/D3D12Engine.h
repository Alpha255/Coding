#pragma once

#include "D3D12Device.h"
#include "D3D12Swapchain.h"

class d3d12Engine : public GfxEngine, public Singleton<d3d12Engine>
{
	singletonDeclare(d3d12Engine);
public:
	void initialize(uint64_t windowHandle, const Configurations &config) override final;
	void finalize() override final;

	static void logError(uint32_t result);

	void handleWindowResize(uint32_t, uint32_t) override final
	{
	}

	void present() override final
	{
	}

	inline void handleWindowResize(uint32_t width, uint32_t height, const Configurations &)
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

	GfxShader *createVertexShader(const std::string &) override final
	{
		return nullptr;
	}
	GfxShader *createFragmentShader(const std::string &) override final
	{
		return nullptr;
	}
protected:
private:
	d3d12Device m_Device;
	d3d12Swapchain m_Swapchain;
};