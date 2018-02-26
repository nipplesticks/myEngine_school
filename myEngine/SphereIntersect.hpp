#ifndef SPHEREINTERSECT_HPP
#define SPHEREINTERSECT_HPP
#include <DirectXMath.h>

class SphereIntersect
{
private: 
	float m_radius;
	DirectX::XMFLOAT3 m_position; 

public:
	SphereIntersect(); 
	SphereIntersect(float radius, DirectX::XMFLOAT3 position); 
	~SphereIntersect(); 

	float getRadius() const; 
	DirectX::XMFLOAT3 getPosition() const; 

	void setPosition(DirectX::XMFLOAT3 position); 

	bool checkIntersection(DirectX::XMFLOAT3 rayOrigin, DirectX::XMFLOAT3 rayDir); 
};

#endif
