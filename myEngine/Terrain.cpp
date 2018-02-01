#include "Terrain.hpp"

Terrain::Terrain()
{
	m_width = 0;
	m_height = 0;
	m_model = new Model;
	m_model->settings(false, false);
}


Terrain::~Terrain()
{
	delete m_model;
}

void Terrain::initTerrainViaHeightMap(std::string path, std::string name, float normalizeHeight, int width, int height, float normalizeUV)
{
	m_width = width;
	m_height = height;
	m_model->initTerrainViaHeightMap(path, name, normalizeHeight, width, height, normalizeUV);
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
