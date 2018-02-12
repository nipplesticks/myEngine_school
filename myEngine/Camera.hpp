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

	DirectX::XMFLOAT3 getPosition() const; 
	DirectX::XMFLOAT3 getForward() const;
	DirectX::XMFLOAT3 getRight() const;
	DirectX::XMFLOAT3 getLookAt() const; 
	DirectX::XMFLOAT3 getUp() const; 

	void setPosition(DirectX::XMFLOAT3 position); 

	CAMERA_BUFFER& getCamBuffer(); 

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

	CAMERA_BUFFER m_camBuffer; 

};

#endif