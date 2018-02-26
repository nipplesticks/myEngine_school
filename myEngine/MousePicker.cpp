#include "MousePicker.hpp"

DirectX::XMFLOAT3 MousePicker::calculateMouseRay()
{
	//Find out the mouse positions
	POINT mousePos; 
	GetCursorPos(&mousePos);
	
	//Mouse position in viewPort space
	float mouseX = (float)mousePos.x; 
	float mouseY = (float)mousePos.y;

	//ViewSpace X and Y 
	DirectX::XMFLOAT4 viewSpaceXYZ; 
	viewSpaceXYZ.x = getNormalizedDeviceCoords(mouseX, mouseY).x;
	viewSpaceXYZ.y = getNormalizedDeviceCoords(mouseX, mouseY).y; 
	//ViewSpace Z
	viewSpaceXYZ.z = 1 / (tan(DirectX::XMConvertToRadians(FOV)) / 2);
	viewSpaceXYZ.w = 1; 

	//From view to worldSpace
	DirectX::XMMATRIX worldSpace = toWorldSpace(); 
	DirectX::XMVECTOR viewSpaceCoords = DirectX::XMLoadFloat4(&viewSpaceXYZ); 
	DirectX::XMVECTOR worldSpaceCoords = DirectX::XMVector2TransformNormal(viewSpaceCoords, worldSpace); 
	




	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT2 MousePicker::getNormalizedDeviceCoords(float mousePosX, float mousePosY)
{
	float x = (2.0f * mousePosX) / (CLIENT_WIDTH - 1.0f); 
	float y = (2.0f * mousePosY) / (CLIENT_HEIGHT - 1.0f); 
	
	return DirectX::XMFLOAT2{ x,y }; 
}

DirectX::XMMATRIX MousePicker::toWorldSpace()
{
	DirectX::XMMATRIX worldMatrix; 
	DirectX::XMVECTOR determinantVector; 
	worldMatrix = DirectX::XMMatrixInverse(&determinantVector, m_viewMatrix); 

	return worldMatrix; 
}

MousePicker::MousePicker(Cam& camera, DirectX::XMMATRIX& projectionMatrix)
{
	m_camera = camera; 
	m_projectionMatrix = projectionMatrix; 

	m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_camera.getPosition()),
												DirectX::XMLoadFloat3(&m_camera.getLookAt()), 
												DirectX::XMLoadFloat3(&m_camera.getUp()));
}

MousePicker::~MousePicker()
{
}

void MousePicker::update()
{
	//Updating viewMatrix every frame
	m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_camera.getPosition()),
											DirectX::XMLoadFloat3(&m_camera.getLookAt()),
											DirectX::XMLoadFloat3(&m_camera.getUp()));
	//Update current ray. 
	m_currentRay = calculateMouseRay(); 
}

DirectX::XMFLOAT3 MousePicker::getRay() const
{
	return m_currentRay; 
}
