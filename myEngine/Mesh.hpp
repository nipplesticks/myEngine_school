#ifndef MESH_HPP
#define MESH_HPP
#include "Material.hpp"

class Mesh
{
public:
	Mesh();
	~Mesh();

	void setVertices(const std::vector<VERTEX>& vertices);
	void setMaterial(const Material& material);
	const std::vector<VERTEX>& getVertices() const;
	const Material& getMaterial() const;
	void buildtangentSpace();

private:
	std::vector<VERTEX> m_vertices;
	Material m_material;
};

#endif