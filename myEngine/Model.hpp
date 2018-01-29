#ifndef MODEL_HPP
#define MODEL_HPP
#include "Structs.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

class Model
{
public:
	Model();
	~Model();

	void settings(bool texture, bool normals, bool clockwise = true);
	void initModel(std::string path);

	void initExistingModel(); 

	std::string getName() const;
	VERTEX* getMesh() const;
	int getNrOfVertices() const;

private:
	VERTEX* m_vertices;
	int m_nrOfVertices;
	std::string m_name;
	bool m_useTexture;
	bool m_useNormals;
	bool m_clockwise;
	bool m_terrainStatus; 
};


#endif