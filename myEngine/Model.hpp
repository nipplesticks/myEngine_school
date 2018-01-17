#ifndef MODEL_HPP
#define MODEL_HPP
#include "Structs.hpp"
#include <string>

class Model
{
public:
	Model();
	~Model();

	void initModel(std::string path);
	
	std::string getName() const;
	VERTEX* getMesh() const;
	int getNrOfVertices() const;

private:
	VERTEX* m_vertices;
	int m_nrOfVertices;
	std::string m_name;
};


#endif