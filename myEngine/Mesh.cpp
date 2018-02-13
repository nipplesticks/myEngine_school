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
		

		XMVECTOR p0 = XMVectorSet(m_vertices[i].x, m_vertices[i].y, m_vertices[i].z, NULL);
		XMVECTOR p1 = XMVectorSet(m_vertices[j].x, m_vertices[j].y, m_vertices[j].z, NULL);
		XMVECTOR p2 = XMVectorSet(m_vertices[k].x, m_vertices[k].y, m_vertices[k].z, NULL);

		XMVECTOR uv0 = XMVectorSet(m_vertices[i].u, m_vertices[i].v, NULL, NULL);
		XMVECTOR uv1 = XMVectorSet(m_vertices[j].u, m_vertices[j].v, NULL, NULL);
		XMVECTOR uv2 = XMVectorSet(m_vertices[k].u, m_vertices[k].v, NULL, NULL);

		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);

		XMVECTOR uvV1 = XMVectorSubtract(uv1, uv0);
		XMVECTOR uvV2 = XMVectorSubtract(uv2, uv0);

		//Magic WTF
		float r = 1.0f / (XMVectorGetX(uvV1) * XMVectorGetY(uvV2) - XMVectorGetX(uvV2) * XMVectorGetY(uvV1));

		XMVECTOR tangent = XMVectorSet(
			r * (XMVectorGetY(uvV2) * XMVectorGetX(v1) - XMVectorGetY(uvV1) * XMVectorGetX(v2)),
			r * (XMVectorGetY(uvV2) * XMVectorGetY(v1) - XMVectorGetY(uvV1) * XMVectorGetY(v2)),
			r * (XMVectorGetY(uvV2) * XMVectorGetZ(v1) - XMVectorGetY(uvV1) * XMVectorGetZ(v2)),
			NULL
		);

		tangent = XMVector3Normalize(tangent);

		XMVECTOR bitangent = XMVectorSet(
			r * ((-1) * XMVectorGetX(uvV2) * XMVectorGetX(v1) + XMVectorGetX(uvV1) * XMVectorGetX(v2)),
			r * ((-1) * XMVectorGetX(uvV2) * XMVectorGetY(v1) + XMVectorGetX(uvV1) * XMVectorGetY(v2)),
			r * ((-1) * XMVectorGetX(uvV2) * XMVectorGetZ(v1) + XMVectorGetX(uvV1) * XMVectorGetZ(v2)),
			NULL
		);
		bitangent = XMVector3Normalize(bitangent);

		for (int counter = 0; counter < 3; counter++)
		{
			int index = i + counter;
			m_vertices[index].xBi = XMVectorGetX(bitangent);
			m_vertices[index].yBi = XMVectorGetY(bitangent);
			m_vertices[index].zBi = XMVectorGetZ(bitangent);

			m_vertices[index].xT = XMVectorGetX(tangent);
			m_vertices[index].yT = XMVectorGetY(tangent);
			m_vertices[index].zT = XMVectorGetZ(tangent);
		}
	}
}
