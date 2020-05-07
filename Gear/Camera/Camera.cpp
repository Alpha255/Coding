#include "Camera.h"
#include "Gear/Window.h"

namespaceStart(Gear)

void Camera::updateKeys(const WindowMessage& message)
{
	switch (message.Key)
	{
	case eKeyboardKey::eKey_A:
	case eKeyboardKey::eKey_Left:
		m_Keys.Left = true;
		break;
	case eKeyboardKey::eKey_D:
	case eKeyboardKey::eKey_Right:
		m_Keys.Right = true;
		break;
	case eKeyboardKey::eKey_W:
	case eKeyboardKey::eKey_Up:
		m_Keys.Up = true;
		break;
	case eKeyboardKey::eKey_S:
	case eKeyboardKey::eKey_Down:
		m_Keys.Right = true;
		break;
	case eKeyboardKey::eKey_Other:
	case eKeyboardKey::eKey_None:
		m_Keys.reset();
		break;
	}
}

void Camera::processMessage(const WindowMessage& message, float32_t elapsedTime, uint32_t width, uint32_t height)
{
	updateKeys(message);

	if (message.State == eWindowState::eResized)
	{
		updatePerspective((width * 1.0f) / (height * 1.0f));
	}

	if (message.Mouse.Left.KeyDown)
	{
	}
	if (message.Mouse.Right.KeyDown)
	{
	}
	if (message.Mouse.Middle.KeyDown)
	{
	}

	if (m_Keys.isMoving())
	{
		Math::Vec3 front(
			-cos(Math::radians(m_Rotation.x)) * sin(Math::radians(m_Rotation.y)),
			sin(Math::radians(m_Rotation.x)),
			cos(Math::radians(m_Rotation.x) * cos(Math::radians(m_Rotation.x))));
		front.normalize();

		float32_t moveSpeed = elapsedTime * m_MoveSpeed;

		if (m_Keys.Up)
		{
			m_Position += front * moveSpeed;
		}
		if (m_Keys.Down)
		{
			m_Position -= front * moveSpeed;
		}
		if (m_Keys.Left)
		{
			Math::Vec3 up(0.0f, 1.0f, 0.0f);
			m_Position -= Math::normalize(Math::cross(up, front)) * moveSpeed;
		}
		if (m_Keys.Right)
		{
			Math::Vec3 up(0.0f, 1.0f, 0.0f);
			m_Position += Math::normalize(Math::cross(up, front)) * moveSpeed;
		}

		updateViewMatrix();
	}
}

void Camera::updateViewMatrix()
{
	auto rotationX = Math::Matrix::rotateX(Math::radians(m_Rotation.x));
	auto rotationY = Math::Matrix::rotateY(Math::radians(m_Rotation.y));
	auto rotationZ = Math::Matrix::rotateZ(Math::radians(m_Rotation.z));

	Math::Matrix rotation = rotationX * rotationY * rotationZ;
	Math::Matrix translation = Math::Matrix::translate(m_Position);

	m_View = translation * rotation;
}

namespaceEnd(Gear)