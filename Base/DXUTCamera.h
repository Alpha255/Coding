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
protected:
	enum eKeyAction
	{
		eUnknown,
		eReset,
		eStrafeLeft,
		eStrafeRight,
		eMoveForward,
		eMoveBackward,
		eMoveUp,
		eMoveDown,
		eControlDown,
		eMaxKeys
	};

	struct MouseAction
	{
		bool LButtonDown = false;
		bool MButtonDown = false;
		bool RButtonDown = false;
		int32_t WheelDelta = 0;
		int32_t ButtonMask = 0;
		int32_t LastPosition[2U] = {};
		Vec2 Delta;
	};

	struct KeyboardAction
	{
		eKeyAction Action = eMaxKeys;
		byte Keys[eMaxKeys] = {};
		uint32_t KeysDown = 0;
		Vec4 Direction;
	};

	eKeyAction GetKeyAction(WPARAM wParam);

	void Reset();
private:
	MouseAction m_MouseAction;

	Vec3 m_Eye;
	Vec3 m_LookAt;

	Vec3 m_DefaultEye;
	Vec3 m_DefaultLookAt;

	float m_FOV = 0.0f;
	float m_Aspect = 1.0f;
	float m_NearPlane = 0.0f;
	float m_FarPlane = 0.0f;

	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;

	Matrix m_View;
	Matrix m_Proj;
};
