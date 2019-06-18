#pragma once

#include "Base/IApplication.h"
#include "Base/Camera.h"
#include "Base/DXUTCamera.h"

NamespaceBegin(Geometry)
	class Model;
NamespaceEnd(Geometry)

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

	virtual void PrepareScene() = 0;
	virtual void RenderScene() = 0;
	virtual void UpdateScene(float, float) {}
protected:
	DXUTCamera m_Camera;

	void AutoFocus(const Geometry::Model &model, float scale);
private:
	Vec2 m_CameraParams = { 0.1f, 500.0f };
};
