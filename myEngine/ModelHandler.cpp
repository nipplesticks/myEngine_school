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

void ModelHandler::loadModel(std::string path, std::string name, bool sTexture, bool sNormals, bool loadWithHeightmap, bool sClockwise)
{
	Model* m = new Model;
	m->settings(sTexture, sNormals, sClockwise);
	if (!loadWithHeightmap)
	{
		m->initModel(path, name);
	}
	else
	{
		m->initTerrainViaHeightMap(path, name);
	}
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
