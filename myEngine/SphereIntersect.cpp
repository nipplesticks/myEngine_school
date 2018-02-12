#include "SphereIntersect.hpp"

SphereIntersect::SphereIntersect()
{
	m_radius = 0; 
	m_position = DirectX::XMFLOAT3(0, 0, 0); 
}

SphereIntersect::SphereIntersect(float radius, DirectX::XMFLOAT3 position)
{
	m_radius = radius; 
	m_position = position; 
}

SphereIntersect::~SphereIntersect()
{
}

float SphereIntersect::getRadius() const
{
	return m_radius; 
}

DirectX::XMFLOAT3 SphereIntersect::getPosition() const
{
	return m_position; 
}

using namespace DirectX;
bool SphereIntersect::checkIntersection(DirectX::XMFLOAT3 rayOrigin, DirectX::XMFLOAT3 rayDir)
{
	//Vector from cameraPos to center.
	DirectX::XMFLOAT3 fromCamPosToCenter = DirectX::XMFLOAT3(m_position.x - rayOrigin.x,m_position.y - rayOrigin.y, m_position.z - rayOrigin.z);
	DirectX::XMVECTOR VfromCamPosToCenter; 
	DirectX::XMVECTOR VrayDir; 
	VfromCamPosToCenter = DirectX::XMLoadFloat3(&fromCamPosToCenter); 
	VrayDir = DirectX::XMLoadFloat3(&rayDir);

	//Lenght of raydir to the point where it is ortogonal to the sphere centre.
	 float lengthOfRayDir = DirectX::XMVectorGetX(DirectX::XMVector3Dot(VfromCamPosToCenter, VrayDir));

	//If lenght of raydir is less than zero, we would not be looking at the sphere. 
	if (lengthOfRayDir < 0) return -1;

	//Knowing the lenght of raydir and the lenght of camPosToCentre, we can figure out the distance from the sphere
	//centre point up to the ortogonally connected point on raydir. We call the distcane dPow2.
	float dPow2 = (DirectX::XMVectorGetX(DirectX::XMVector3Dot(VfromCamPosToCenter, VfromCamPosToCenter)) - (lengthOfRayDir * lengthOfRayDir));

	//If dPow2 is larger than the radius of the sphere, that means we have missed the sphere. 
	if (dPow2 >(m_radius * m_radius)) return false;
	return true; 
}

void SphereIntersect::setPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}
