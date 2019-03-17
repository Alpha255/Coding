#include "IRenderApplication.h"
#include "Definitions.h"

void IRenderApplication::Initialize(const std::string &title, uint32_t width, uint32_t height, bool bFullScreen, uint32_t extraWindowStyle)
{
	m_IconID = IconID;
	Base::Initialize(title, width, height, bFullScreen, extraWindowStyle);

	REngine::Instance().Initialize(m_hWnd, width, height, bFullScreen);
}

void IRenderApplication::HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleWindowMessage(msg, wParam, lParam);
}

void IRenderApplication::HandleInput(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
{
	Base::HandleInput(msg, wParam, lParam);

	switch (msg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		HandleMouseInput(wParam, lParam, eMouseButtonDown);
		break;
	case WM_MOUSEMOVE:
		HandleMouseInput(wParam, lParam, eMouseMove);
		break;
	case WM_MOUSEWHEEL:
		HandleMouseInput(wParam, lParam, eMouseWheel);
		break;
	}
}

void IRenderApplication::HandleMouseInput(::WPARAM wParam, ::LPARAM lParam, eMouseInput type)
{
	m_MousePos.first = (int32_t)(LOWORD(lParam));
	m_MousePos.second = (int32_t)(HIWORD(lParam));

	m_MouseWheel = (int16_t)(HIWORD(wParam));

	switch (type)
	{
	case eMouseMove:
		break;
	case eMouseButtonDown:
		break;
	case eMouseWheel:
		break;
	}
}

void IRenderApplication::ResizeWindow(uint32_t width, uint32_t height)
{
	Base::ResizeWindow(width, height);

	REngine::Instance().Resize(width, height);
}

void IRenderApplication::RenterToWindow()
{

}

void IRenderApplication::Finalize()
{

}