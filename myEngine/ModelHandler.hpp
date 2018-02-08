#ifndef MODELHANDLER_HPP
#define MODELHANDLER_HPP
#include "Model.hpp"
#include <vector>

class ModelHandler
{
public:
	ModelHandler();
	~ModelHandler();

	void loadModel(std::string path, std::string name, ID3D11Device* device, bool sTexture, bool sNormals, bool sClockwise = true);
	Model* getModel(std::string name);

private:
	std::vector<Model*> models;
};
	


#endif