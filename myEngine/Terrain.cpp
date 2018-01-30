#include "Terrain.h"

Terrain::Terrain()
{ 
	this->InitTerrainModel();
}

Terrain::~Terrain()
{
	
}

void Terrain::InitTerrainModel()
{
	this->terrainFile.open("HeightMap.bmp", 0); 
	if (this->terrainFile.is_open())
	{
		for (int i = 0; i < 513; i++)
		{
			for (int j = 0; j < 513; j++)
			{
				this->heightMap[i][j] = this->terrainFile.get(); 
				std::cout << this->heightMap[i][j];
			}
		}
	}
	this->terrainFile.close(); 
}
