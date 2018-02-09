#ifndef CHARACTER_H 
#define CHARACTER_H
#include <string>
#include "Terrain.hpp"

class Character : public Entity
{
public:
	Character(std::string modelName, Model* model); 
	Character(); 
	~Character(); 

	void collisionHandling(const Terrain& mapModel, int mapWidth); 
private: 

};

#endif
