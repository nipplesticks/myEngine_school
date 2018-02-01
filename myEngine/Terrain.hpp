#ifndef TERRAIN_HPP
#define TERRAIN_HPP
#include "Entity.hpp"

class Terrain :
	public Entity
{
public:
	Terrain();
	~Terrain();

	void initTerrainViaHeightMap(std::string path, std::string name, float normalizeHeight, int width, int height, float normalizeUV);
	void setTerrainTexture(wchar_t* path, ID3D11Device*& device, int textureSetting = 0);
	int getWidith() const;
	int getHeight() const;

private:
	int m_width;
	int m_height;
};

#endif