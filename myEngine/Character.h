#ifndef CHARACTER_H 
#define CHARACTER_H
#include <string>
#include "Entity.hpp"

class Character : public Entity
{
public:
	Character(std::string modelName, Model* model); 
	Character(); 
	~Character(); 

private: 

};

#endif
