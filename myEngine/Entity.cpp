#include "Entity.hpp"

Entity::Entity()
{
	m_polygons = nullptr;

	m_nrOfPolygons = 3;

	m_polygons = new VERTEX[m_nrOfPolygons];

	m_polygons[0] = VERTEX{
		-0.5, -0.5, 0.0,
		0.0, 0.0
	};
	m_polygons[1] = VERTEX{
		0.5, -0.5, 0.0,
		0.0, 0.0
	};
	m_polygons[0] = VERTEX{
		-0.5, 0.5, 0.0,
		0.0, 0.0
	};

}

Entity::~Entity()
{
	delete[] m_polygons;
	m_polygons = nullptr;
}

VERTEX * Entity::getPolygons() const
{
	return m_polygons;
}

int Entity::getNrOfPolygons() const
{
	return m_nrOfPolygons;
}
