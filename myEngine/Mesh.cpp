#include "Mesh.hpp"

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
	
}

void Mesh::setVertices(const std::vector<VERTEX>& vertices)
{
	m_vertices = vertices;
}

void Mesh::setMaterial(const Material& material)
{
	m_material = material;
}

const std::vector<VERTEX>& Mesh::getVertices() const
{
	return m_vertices;
}

const Material & Mesh::getMaterial() const
{
	return m_material;
}
