#ifndef CAMERA_HPP
#define CAMERA_HPP
#include "Structs.hpp"


class Cam
{
public:
	Cam(DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 lookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT3 up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	~Cam() {};
	
	void update();

	DirectX::XMMATRIX getViewMatrix();
	DirectX::XMMATRIX getViewMatrixForBackground();
private:
	void moveX(int leftOrRight);
	void moveZ(int forwardOrBackwards);
	void moveY(int upOrDown);

private:
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_lookAt;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_right;
	DirectX::XMFLOAT3 m_forward;
	DirectX::XMFLOAT2 m_rotation;

	float m_defaultX;
	float m_defaultY;

	float m_speed;
	float m_speedChanger;
	float m_sensitivity;

	bool m_crouchLock;
	bool m_flying;

};





#endif