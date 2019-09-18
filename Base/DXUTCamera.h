#pragma once

#include "Base.h"

class DXUTCamera
{
public:
	DXUTCamera();
	~DXUTCamera() = default;

	inline void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam)
	{
		m_KeyAction.HandleWindowMessage(msg, wParam, lParam);

		m_MouseAction.HandleWindowMessage(msg, wParam, lParam);
	}

	void SetViewParams(const Vec3 &eye, const Vec3 &lookAt);
	void SetProjParams(float fov, float aspect, float nearPlane, float farPlane);
	void Update(float elapsedTime);

	inline Matrix GetWorldMatrix() const 
	{
		return m_World;
	}

	inline Matrix GetViewMatrix() const 
	{
		return m_View;
	}

	inline Matrix GetProjMatrix() const 
	{
		return m_Proj;
	}

	inline Matrix GetWVPMatrix() const 
	{
		return m_View * m_Proj;
	}

	inline Vec3 GetEyePos() const
	{
		return m_Eye;
	}

	inline void SetScalers(float rotateScaler = 0.01f, float moveScaler = 5.0f)
	{
		m_MouseAction.SetScalers(rotateScaler, moveScaler);
	}
protected:

	struct MouseAction
	{
		enum eButton
		{
			eLeftButton,
			eMiddleButton,
			eRightButton,
			eButtonCount
		};

		inline void SetRotateButton(eButton button)
		{
			assert(RotateButton != eButtonCount);
			RotateButton = button;
		}

		inline bool IsRotateButtonDown()
		{
			assert(RotateButton != eButtonCount);
			return ButtonDown[RotateButton];
		}

		inline void SetScalers(float rotateScaler, float moveScaler)
		{
			RotateScaler = rotateScaler;
			MoveScaler = moveScaler;
		}

		void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
		void UpdateInput();

		bool ButtonDown[eButtonCount] = {};
		int32_t WheelDelta = 0;
		eButton RotateButton = eLeftButton;
		int32_t LastPosition[2U] = {};
		uint32_t FrameCountToSmoothMouseData = 2U;
		float RotateScaler = 0.01f;
		float MoveScaler = 5.0f;
		Vec2 Delta;
		Vec2 RotateVelocity;
	};

	struct KeyboardAction
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

		eKeyAction GetKeyAction(WPARAM wParam);

		inline bool IsKeyActive(eKeyAction action) const
		{
			return ActiveKeys[action];
		}

		inline void ActiveKey(eKeyAction action)
		{
			ActiveKeys[action] = true;
		}

		inline void InactiveKey(eKeyAction action)
		{
			ActiveKeys[action] = false;
		}

		void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
		void UpdateInput();

		Vec3 KeyDirection;
		bool EnableYAxisMovement = true;
		bool ActiveKeys[eOther] = {};
	};

	inline void Reset()
	{
		SetViewParams(m_DefaultEye, m_DefaultLookAt);
	}
	
	inline void UpdateInput()
	{
		m_KeyAction.UpdateInput();

		m_MouseAction.UpdateInput();
	}

	void UpdateVelocity(float elapsedTime);
private:
	MouseAction m_MouseAction;
	KeyboardAction m_KeyAction;

	Vec3 m_Eye;
	Vec3 m_LookAt;

	Vec3 m_DefaultEye;
	Vec3 m_DefaultLookAt;

	Vec3 m_Velocity;
	Vec3 m_VelocityDrag;

	float m_FOV = 0.0f;
	float m_Aspect = 1.0f;
	float m_NearPlane = 0.0f;
	float m_FarPlane = 0.0f;

	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;

	float m_DragTimer = 0.0f;

	Matrix m_World;
	Matrix m_View;
	Matrix m_Proj;
};
