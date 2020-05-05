#pragma once

#include "Gear/Math/Matrix.h"

namespaceStart(Gear)

class dxutCamera
{
public:
	dxutCamera();
	~dxutCamera() = default;

	inline void processEvent()
	{
		m_KeyAction.processEvent();

		m_MouseAction.processEvent();
	}

	void setViewParams(const Math::Vec3 &eye, const Math::Vec3 &lookAt);
	void setProjParams(float32_t fov, float32_t aspect, float32_t nearPlane, float32_t farPlane);
	void update(float32_t elapsedTime);

	inline const Math::Matrix &worldMatrix() const 
	{
		return m_World;
	}

	inline const Math::Matrix &viewMatrix() const 
	{
		return m_View;
	}

	inline const Math::Matrix &projMatrix() const 
	{
		return m_Proj;
	}

	inline Math::Matrix WVPMatrix() const
	{
		return m_View * m_Proj;
	}

	inline const Math::Vec3 &eye() const
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
			eWheel,
			eButtonCount
		};

		inline void setRotateButton(eButton button)
		{
			assert(RotateButton != eButtonCount);
			RotateButton = button;
		}

		inline bool8_t isRotateButtonDown()
		{
			assert(RotateButton != eButtonCount);
			return ButtonDown[RotateButton];
		}

		inline void setScalers(float32_t rotateScaler, float32_t moveScaler)
		{
			RotateScaler = rotateScaler;
			MoveScaler = moveScaler;
		}

		void processEvent();
		void updateInput();

		bool8_t ButtonDown[eButtonCount] = {};
		float32_t WheelDelta = 0.0f;
		eButton RotateButton = eLeftButton;
		uint32_t FrameCountToSmoothMouseData = 2u;
		float32_t RotateScaler = 0.01f;
		float32_t MoveScaler = 5.0f;
		Math::Vec2 LastPosition;
		Math::Vec2 Delta;
		Math::Vec2 RotateVelocity;
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

		eKeyAction getKeyAction();

		inline bool8_t isKeyActive(eKeyAction action) const
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

		void processEvent();
		void updateInput();

		Math::Vec3 KeyDirection;
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

	Math::Vec3 m_Eye;
	Math::Vec3 m_LookAt;

	Math::Vec3 m_DefaultEye;
	Math::Vec3 m_DefaultLookAt;

	Math::Vec3 m_Velocity;
	Math::Vec3 m_VelocityDrag;

	float32_t m_FOV = 0.0f;
	float32_t m_Aspect = 1.0f;
	float32_t m_NearPlane = 0.0f;
	float32_t m_FarPlane = 0.0f;

	float32_t m_Yaw = 0.0f;
	float32_t m_Pitch = 0.0f;

	float32_t m_DragTimer = 0.0f;

	Math::Matrix m_World;
	Math::Matrix m_View;
	Math::Matrix m_Proj;
};

namespaceEnd(Gear)
