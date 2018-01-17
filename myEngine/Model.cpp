#include "Model.hpp"

Model::Model()
{
	m_vertices = nullptr;
	m_nrOfVertices = 0;
	m_name = "";
}

Model::~Model()
{
	delete[] m_vertices;
}

void Model::initModel(std::string path)
{
	m_nrOfVertices = 3;
	m_vertices = new VERTEX[m_nrOfVertices];
	
	m_vertices[0] = VERTEX{
		-0.5f, 0.5f, 0.0f,
		0.0f, 0.0f
	};
	m_vertices[1] = VERTEX{
		0.5f, 0.5f, 0.0f,
		0.0f, 0.0f,
	};
	m_vertices[2] = VERTEX{
		-0.5f, -0.5f, 0.0f,
		0.0f, 0.0f
	};
}

std::string Model::getName() const
{
	return m_name;
}

VERTEX * Model::getMesh() const
{
	return m_vertices;
}

int Model::getNrOfVertices() const
{
	return m_nrOfVertices;
}
