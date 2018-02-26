#include "ModelHandler.hpp"

ModelHandler::ModelHandler()
{
}

ModelHandler::~ModelHandler()
{
	int nrOfModels = static_cast<int>(models.size());
	for (int i = 0; i < nrOfModels; i++)
	{
		delete models[i];
	}
	for (int i = 0; i < nrOfModels; i++)
	{
		models.pop_back();
	}
}

void ModelHandler::loadModel(std::string path, std::string name, ID3D11Device* device, bool sTexture, bool sNormals, bool sClockwise)
{
	Model* m = new Model();
	m->settings(sTexture, sNormals, sClockwise);
	m->initModel(path, name, device);
	models.push_back(m);
}

Model * ModelHandler::getModel(std::string name)
{
	Model* reqModel = nullptr;
	bool found = false;
	std::string mName = "";
	for (unsigned int i = 0; i < models.size() && !found; i++)
	{
		mName = models[i]->getName();
		if (mName == name)
		{
			reqModel = models[i];
			found = true;
		}
	}

	return reqModel;
}
