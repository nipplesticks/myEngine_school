#ifndef TERRAIN_H
#define TERRAIN_H
#include "Entity.hpp"
#include <fstream>
class Terrain : public Entity
{
public: 
	Terrain(); 
	~Terrain(); 
private:
	void InitTerrainModel(); 
	std::ifstream terrainFile; 
	
	char heightMap[512][512]; 
}; 
#endif
