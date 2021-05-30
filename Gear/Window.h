#pragma once

#include "Gear/InputState.h"

NAMESPACE_START(Gear)

class Window
{
public:
	enum class EState : uint8_t
	{
		Minimized,
		Maximized,
		Restored,
		Fullscreen,
		Destroyed,
	};

	Window(uint64_t instance, const std::string& title, const Math::Vec2& size, const Math::Vec2& minSize);

	inline const uint32_t width() const
	{
		return static_cast<uint32_t>(m_Size.x);
	}

	inline const uint32_t height() const
	{
		return static_cast<uint32_t>(m_Size.y);
	}

	inline const uint64_t handle() const
	{
		return m_Handle;
	}

	inline const bool8_t isActive() const
	{
		return m_Active;
	}

	inline const EState state() const
	{
		return m_State;
	}

	void processMessage(uint32_t message, size_t wParam, intptr_t lParam);

	void update();
protected:
	void updateSize();
private:
	Math::Vec2 m_Size;
	Math::Vec2 m_MinSize;
	bool8_t m_Active = true;
	EState m_State = EState::Restored;
	uint64_t m_Handle = 0u;
};
DECLARE_UNIQUE_PTR(Window)

NAMESPACE_END(Gear)
