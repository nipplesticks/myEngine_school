#include "Terrain.hpp"

Terrain::Terrain()
{
	m_width = 0;
	m_height = 0;
	m_model = new Model;
	m_heigtNormalisor;
	m_model->settings(false, false);
}


Terrain::~Terrain()
{
	delete m_model;
	for (int i = 0; i < m_height; i++)
	{
		delete m_hightValues[i];
	}
	delete[] m_hightValues;

}

void Terrain::initTerrainViaHeightMap(std::string path, std::string name, float normalizeHeight, int width, int height, float normalizeUV)
{
	m_width = width;
	m_height = height;
	m_heigtNormalisor = normalizeHeight;

	m_model->initTerrainViaHeightMap(path, name, normalizeHeight, width, height, normalizeUV, m_hightValues);
}

void Terrain::setTerrainTexture(wchar_t * path, ID3D11Device *& device, int textureSetting)
{
	m_model->settings(true, false);
	m_model->initTexture(path, device, textureSetting);
}

int Terrain::getWidith() const
{
	return m_width;
}

int Terrain::getHeight() const
{
	return m_height;
}

float ** Terrain::getHightValues() const
{
	return m_hightValues;
}

float Terrain::getDeltaHight() const
{
	return m_heigtNormalisor;
}
