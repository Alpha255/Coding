#pragma once

#include "Base.h"

class DXUTCamera
{
public:
	DXUTCamera();
	~DXUTCamera() = default;

	void HandleWindowMessage(uint32_t msg, ::WPARAM wParam, ::LPARAM lParam);
	void SetViewParams(const Vec3 &eye, const Vec3 &lookAt);
	void SetProjParams(float fov, float aspect, float nearPlane, float farPlane);
	void Update(float elapsedTime);
protected:

	struct MouseAction
	{
		enum eMouseKeyActionMask
		{
			eLeftButton = 0x01,
			eMiddleButton = 0x02,
			eRightButton = 0x04,
			eWheel = 0x08
		};

		inline void SetRotateButton(eMouseKeyActionMask mask)
		{
			RotateButtonMask = (int32_t)mask;
		}

		bool LButtonDown = false;
		bool MButtonDown = false;
		bool RButtonDown = false;
		int32_t WheelDelta = 0;
		int32_t ButtonMask = 0;
		int32_t LastPosition[2U] = {};
		int32_t RotateButtonMask = eLeftButton;
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
			eMaxKeys,
			eOther
		};

		eKeyAction GetKeyAction(WPARAM wParam);

		inline bool IsKeyDown(eKeyAction action) const
		{
			return Keys[action];
		}

		inline void KeyDown(eKeyAction action)
		{
			Keys[action] = true;
			++KeysDown;
		}

		inline void KeyUp(eKeyAction action)
		{
			Keys[action] = false;
			--KeysDown;
		}

		Vec3 KeyDirection;
		bool EnableYAxisMovement = true;
		bool Keys[eMaxKeys] = {};
		uint32_t KeysDown = 0;
	};

	inline void Reset()
	{
		SetViewParams(m_DefaultEye, m_DefaultLookAt);
	}

	void UpdateInput();
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

	Matrix m_View;
	Matrix m_Proj;
};
