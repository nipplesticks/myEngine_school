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
	bool intersection = false; 
	float det, b, i1, i2; 
	XMVECTOR diffRaySphere = XMVectorSubtract(XMLoadFloat3(&rayOrigin),XMLoadFloat3(&m_position));
	b = XMVectorGetX(XMVector3Dot(diffRaySphere, XMLoadFloat3(&rayDir))) * - 1; 
	det = b * b - XMVectorGetX(XMVector3Dot(diffRaySphere, diffRaySphere)) + m_radius * m_radius; 
	if (det < 0)
		return false; 
	det = (float)sqrt(det); 
	i1 = b - det; 
	i2 = b + det; 
	//Intersecting with ray?
	if (i2 < 0)
		return false; 
	if (i1 < 0) 
		i1 = 0; 

	return true; 
}

void SphereIntersect::setPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}
