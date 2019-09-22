#pragma once

#include "gear/Math/Matrix.h"
#include "gear/Math/Functions.h"

namespaceStart(gear)

class dxutCamera
{
public:
	dxutCamera();
	~dxutCamera() = default;

	inline void handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
	{
		m_KeyAction.handleMessage(msg, wParam, lParam);

		m_MouseAction.handleMessage(msg, wParam, lParam);
	}

	void setViewParams(const math::vec3 &eye, const math::vec3 &lookAt);
	void setProjParams(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane);
	void update(float32_t elapsedTime);

	inline const math::matrix &worldMatrix() const 
	{
		return m_World;
	}

	inline const math::matrix &viewMatrix() const 
	{
		return m_View;
	}

	inline const math::matrix &projMatrix() const 
	{
		return m_Proj;
	}

	inline const math::matrix &WVPMatrix() const
	{
		return m_View * m_Proj;
	}

	inline const math::vec3 &eye() const
	{
		return m_Eye;
	}

	inline void setScalers(float32_t rotateScaler = 0.01f, float32_t moveScaler = 5.0f)
	{
		m_MouseAction.setScalers(rotateScaler, moveScaler);
	}
protected:

	struct mouseAction
	{
		enum eButton
		{
			eLeftButton,
			eMiddleButton,
			eRightButton,
			eButtonCount
		};

		inline void setRotateButton(eButton button)
		{
			assert(RotateButton != eButtonCount);
			RotateButton = button;
		}

		inline bool isRotateButtonDown()
		{
			assert(RotateButton != eButtonCount);
			return ButtonDown[RotateButton];
		}

		inline void setScalers(float32_t rotateScaler, float32_t moveScaler)
		{
			RotateScaler = rotateScaler;
			MoveScaler = moveScaler;
		}

		void handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
		void updateInput();

		bool8_t ButtonDown[eButtonCount] = {};
		int32_t WheelDelta = 0;
		eButton RotateButton = eLeftButton;
		int32_t LastPosition[2U] = {};
		uint32_t FrameCountToSmoothMouseData = 2U;
		float32_t RotateScaler = 0.01f;
		float32_t MoveScaler = 5.0f;
		math::vec2 Delta;
		math::vec2 RotateVelocity;
	};

	struct keyboardAction
	{
		enum eKeyAction
		{
			eReset,
			eStrafeLeft,
			eStrafeRight,
			eMoveForward,
			eMoveBackward,
			eMoveUp,
			eMoveDown,
			eControlDown,
			eOther
		};

		eKeyAction keyAction(WPARAM wParam);

		inline bool isKeyActive(eKeyAction action) const
		{
			return ActiveKeys[action];
		}

		inline void activeKey(eKeyAction action)
		{
			ActiveKeys[action] = true;
		}

		inline void inactiveKey(eKeyAction action)
		{
			ActiveKeys[action] = false;
		}

		void handleMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
		void updateInput();

		math::vec3 KeyDirection;
		bool8_t EnableYAxisMovement = true;
		bool8_t ActiveKeys[eOther] = {};
	};

	inline void reset()
	{
		setViewParams(m_DefaultEye, m_DefaultLookAt);
	}
	
	inline void updateInput()
	{
		m_KeyAction.updateInput();

		m_MouseAction.updateInput();
	}

	void updateVelocity(float32_t elapsedTime);
private:
	mouseAction m_MouseAction;
	keyboardAction m_KeyAction;

	math::vec3 m_Eye;
	math::vec3 m_LookAt;

	math::vec3 m_DefaultEye;
	math::vec3 m_DefaultLookAt;

	math::vec3 m_Velocity;
	math::vec3 m_VelocityDrag;

	float32_t m_FOV = 0.0f;
	float32_t m_Aspect = 1.0f;
	float32_t m_NearPlane = 0.0f;
	float32_t m_FarPlane = 0.0f;

	float32_t m_Yaw = 0.0f;
	float32_t m_Pitch = 0.0f;

	float32_t m_DragTimer = 0.0f;

	math::matrix m_World;
	math::matrix m_View;
	math::matrix m_Proj;
};

namespaceEnd(gear)
