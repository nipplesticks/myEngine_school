#include "Character.h"

Character::Character(std::string modelName, Model* model) : Entity()
{ 
	model->settings(true,true);
	model->initModel("models/" + modelName + ".obj"); 
}

Character::Character() : Entity()
{
}

Character::~Character()
{

}
