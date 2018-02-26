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

using namespace DirectX;
void Mesh::buildtangentSpace()
{
	for (int i = 0; i < (int)m_vertices.size(); i += 3)
	{
		int j = i + 1;
		int k = i + 2;
		
		XMVECTOR v0 = XMVectorSet(m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, NULL);
		XMVECTOR v1 = XMVectorSet(m_vertices[j].x, m_vertices[j].y, m_vertices[j].z, NULL);
		XMVECTOR v2 = XMVectorSet(m_vertices[k].x, m_vertices[k].y, m_vertices[k].z, NULL);

		XMVECTOR edge1 = XMVectorSubtract(v1, v0);
		XMVECTOR edge2 = XMVectorSubtract(v2, v0);

		float deltaU1 = m_vertices[j].u - m_vertices[i].u;
		float deltaV1 = m_vertices[j].v - m_vertices[i].v;
		float deltaU2 = m_vertices[k].u - m_vertices[i].u;
		float deltaV2 = m_vertices[k].v - m_vertices[i].v;

		XMFLOAT3 tangent, bitangent;

		tangent.x = deltaV2 * XMVectorGetX(edge1) - deltaV1 * XMVectorGetX(edge2);
		tangent.y = deltaV2 * XMVectorGetY(edge1) - deltaV1 * XMVectorGetY(edge2);
		tangent.z = deltaV2 * XMVectorGetZ(edge1) - deltaV1 * XMVectorGetZ(edge2);

		bitangent.x = -deltaU2 * XMVectorGetX(edge1) - deltaU1 * XMVectorGetX(edge2);
		bitangent.y = -deltaU2 * XMVectorGetY(edge1) - deltaU1 * XMVectorGetY(edge2);
		bitangent.z = -deltaU2 * XMVectorGetZ(edge1) - deltaU1 * XMVectorGetZ(edge2);

		XMVECTOR vTangent, vBitangent;

		vTangent = XMVector3Normalize(XMLoadFloat3(&tangent));
		vBitangent = XMVector3Normalize(XMLoadFloat3(&bitangent));

		for (int counter = 0; counter < 3; counter++)
		{
			int index = i + counter;
			m_vertices[index].xBi = XMVectorGetX(vBitangent);
			m_vertices[index].yBi = XMVectorGetY(vBitangent);
			m_vertices[index].zBi = XMVectorGetZ(vBitangent);

			m_vertices[index].xT = XMVectorGetX(vTangent);
			m_vertices[index].yT = XMVectorGetY(vTangent);
			m_vertices[index].zT = XMVectorGetZ(vTangent);
		}
	
	}
}

void Mesh::releaseMaterial()
{
	m_material.release();
}
