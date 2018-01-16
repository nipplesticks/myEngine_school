#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "Structs.hpp"


class Entity
{
public:
	Entity();
	~Entity();
	VERTEX* getPolygons() const;
	int getNrOfPolygons() const;

private:
	VERTEX* m_polygons;
	int m_nrOfPolygons;
	DirectX::XMVECTOR m_pos;
	DirectX::XMVECTOR m_rot;

};

#endif