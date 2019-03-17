#pragma once

#include "Base/IApplication.h"
#include "Base/Camera.h"

class IRenderApplication : public IApplication
{
public:
	IRenderApplication() = default;
	virtual ~IRenderApplication() = default;

	void Initialize(const std::string &title, uint32_t width = 1280U, uint32_t height = 720U, bool bFullScreen = false, uint32_t extraWindowStyle = 0U) override;

	void Finalize() override;

	void ResizeWindow(uint32_t width, uint32_t height) override;

	void RenterToWindow() override;

	void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam) override;

	void HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam) override;
protected:
	enum eMouseInput
	{
		eMouseMove,
		eMouseButtonDown,
		eMouseWheel
	};

	void HandleMouseInput(::WPARAM wParam, ::LPARAM lParam, eMouseInput type);

	Camera m_Camera;
private:
	std::pair<int32_t, int32_t> m_MousePos;
	int16_t m_MouseWheel = 0;
};
