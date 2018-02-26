#include "Camera.hpp"

using namespace DirectX;
Cam::Cam(XMFLOAT3 position, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	m_position = position;
	m_lookAt = lookAt;
	m_up = up;

	// Here we calculate the start rotation based on the lookAt vector in x rotation
	m_rotation.x =
		DirectX::XMConvertToDegrees(
			DirectX::XMVectorGetX(
				DirectX::XMVector3Dot(
					DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, NULL),
					DirectX::XMLoadFloat3(&lookAt)))
		);
	// Here we calculate the start rotation based on the lookAt vector in y rotation
	m_rotation.y = DirectX::XMConvertToDegrees(
		DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, NULL),
				DirectX::XMLoadFloat3(&lookAt)))
	);

	// Calculate the right vector
	XMVECTOR v1 = XMLoadFloat3(&m_lookAt);
	XMVECTOR v2 = XMLoadFloat3(&m_up);
	XMVECTOR v3 = XMVector3Normalize(XMVector3Cross(v2, v1));
	XMStoreFloat3(&m_right, v3);

	// Set the default value to the middle of the screen
	m_defaultX = (CLIENT_WIDTH) / 2;
	m_defaultY = (CLIENT_HEIGHT) / 2;

	// Movement speed and rotation speed
	m_speed = 5.0f;
	m_speedChanger = 0.0f;
	m_sensitivity = 0.05f;
	m_crouchLock = false;
	m_flying = true;
}

void Cam::update()
{
	m_speedChanger = 1.0f;
	
	XMVECTOR lookAt = XMLoadFloat3(&m_lookAt);
	XMVECTOR up = XMLoadFloat3(&m_up);
	
	// Calculate the right vector
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, lookAt));
	
	// Calculate the forward vector
	XMVECTOR forward = XMVector3Cross(up, right);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_forward, forward);


	// Some movement changing values
	if (GetAsyncKeyState(VK_LSHIFT)) m_speedChanger = 2.0f;
	if (!m_flying && GetAsyncKeyState(VK_LCONTROL)) m_speedChanger = 1.0f * 0.4f;


	// Movement control
	if (GetAsyncKeyState(int('A')))
		moveX(-1);
	if (GetAsyncKeyState(int('D')))
		moveX(1);
	if (GetAsyncKeyState(int('W')))
		moveZ(1);
	if (GetAsyncKeyState(int('S')))
		moveZ(-1);

	if (!m_flying)
	{
		if (GetAsyncKeyState(VK_LCONTROL))
		{
			if (!m_crouchLock)
			{
				m_crouchLock = true;
				m_position.x -= m_up.x * 2;
				m_position.y -= m_up.y * 2;
				m_position.z -= m_up.z * 2;
			}
		}
		else if (m_crouchLock)
		{
			m_crouchLock = false;
			m_position.x += m_up.x * 2;
			m_position.y += m_up.y * 2;
			m_position.z += m_up.z * 2;
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_SPACE))
		{
			moveY(1);
		}
		else if (GetAsyncKeyState(VK_LCONTROL))
		{
			moveY(-1);
		}
	}
	

	// Mouse rotation magic
	POINT mousePos;
	GetCursorPos(&mousePos);
	if (mousePos.x != m_defaultX || mousePos.y != m_defaultY)
	{
		// Get the delta vectors
		float deltaX = m_defaultX - mousePos.x;
		float deltaY = m_defaultY - mousePos.y;

		// Make angle out of the vector
		m_rotation.x += deltaX * m_sensitivity;
		m_rotation.y += deltaY * m_sensitivity;

		if (m_rotation.y > 88.0f)
			m_rotation.y = 88.0f;
		else if (m_rotation.y < -88.0f)
			m_rotation.y = -88.0f;

		// Here we calculate the new values of the lookAt vector
		XMFLOAT3 la;
		la.x = cos(XMConvertToRadians(m_rotation.x)) * cos(XMConvertToRadians(m_rotation.y));
		la.y = sin(XMConvertToRadians(m_rotation.y));
		la.z = sin(XMConvertToRadians(m_rotation.x)) * cos(XMConvertToRadians(m_rotation.y));

		XMVECTOR newLookAt = XMVector3Normalize(XMLoadFloat3(&la));
		XMStoreFloat3(&m_lookAt, newLookAt);
	}
	SetCursorPos((int)m_defaultX, (int)m_defaultY); // Set cursor pos to the middle of the screen
}

XMMATRIX Cam::getViewMatrix()
{
	return  XMMatrixLookAtLH(
		XMLoadFloat3(&m_position),
		XMVectorAdd(XMLoadFloat3(&m_lookAt), XMLoadFloat3(&m_position)),
		XMLoadFloat3(&m_up)
	);
}

DirectX::XMMATRIX Cam::getViewMatrixForBackground()
{
	// Returns a view matrix which lets the camera "pretend" it's in origo. This makes objects
	// "stick" at a position so the camera can never reaches it.
	return XMMatrixLookAtLH(
		XMVectorZero(),
		XMLoadFloat3(&m_lookAt),
		XMLoadFloat3(&m_up)
	);
}

DirectX::XMFLOAT3 Cam::getPosition() const
{
	return m_position; 
}

DirectX::XMFLOAT3 Cam::getForward() const
{
	return m_forward;
}

DirectX::XMFLOAT3 Cam::getRight() const
{
	return m_right;
}

DirectX::XMFLOAT3 Cam::getLookAt() const
{
	return m_lookAt; 
}

DirectX::XMFLOAT3 Cam::getUp() const
{
	return m_up;
}

void Cam::setPosition(DirectX::XMFLOAT3 position)
{
	m_position = position; 
}
CAMERA_BUFFER & Cam::getCamBuffer()
{
	return m_camBuffer; 
}
void Cam::moveX(int leftOrRight)
{
	m_position.x += leftOrRight * (m_speed * m_speedChanger * m_right.x);
	m_position.y += leftOrRight * (m_speed * m_speedChanger * m_right.y);
	m_position.z += leftOrRight * (m_speed * m_speedChanger * m_right.z);
}

void Cam::moveZ(int forwardOrBackwards)
{
	XMFLOAT3 dir = m_forward;
	if (m_flying)
		dir = m_lookAt;

	m_position.x += forwardOrBackwards * (m_speed * m_speedChanger * dir.x);
	m_position.y += forwardOrBackwards * (m_speed * m_speedChanger * dir.y);
	m_position.z += forwardOrBackwards * (m_speed * m_speedChanger * dir.z);
}

void Cam::moveY(int upOrDown)
{
	m_position.x += upOrDown * (m_speed * m_speedChanger * m_up.x);
	m_position.y += upOrDown * (m_speed * m_speedChanger * m_up.y);
	m_position.z += upOrDown * (m_speed * m_speedChanger * m_up.z);
}
