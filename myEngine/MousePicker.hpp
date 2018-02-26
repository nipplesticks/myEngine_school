#ifndef MOUSEPICKER_HPP
#define MOUSEPICKER_HPP
#include <DirectXMath.h>
#include "Camera.hpp"

class MousePicker
{
private: 
	DirectX::XMFLOAT3 m_currentRay;
	DirectX::XMMATRIX m_projectionMatrix; 
	DirectX::XMMATRIX m_viewMatrix;
	Cam m_camera;
	
	//In this function, the 2D mouse position gets converted into a 3D world space ray.
	DirectX::XMFLOAT3 calculateMouseRay(); 

	DirectX::XMFLOAT2 getNormalizedDeviceCoords(float mousePosx, float mousePosY); 
	DirectX::XMMATRIX toWorldSpace(); 

public: 

	MousePicker(Cam& camera, DirectX::XMMATRIX& projectionMatrix); 
	~MousePicker();

	void update(); 

	DirectX::XMFLOAT3 getRay() const; 

};

#endif
