#include "Model.hpp"
#include "TerrainLoader.hpp"
#include <iostream>

Model::Model()
{
	m_vertices = nullptr;
	m_nrOfVertices = 0;
	m_name = "";
	m_useTexture = false;
	m_useNormals = false;
}

Model::~Model()
{
	delete[] m_vertices;
}

void Model::settings(bool texture, bool normals, bool clockwise)
{
	m_useTexture = texture;
	m_useNormals = normals;
	m_clockwise = clockwise;
}

void Model::initModel(std::string path)
{
	if (!m_terrainStatus)
	{
		std::ifstream mod;
		std::cout << "Importing " << path << std::endl;
		mod.open(path);
		if (mod)
		{
			std::string currentLine = "";
			std::string type = "";
			std::vector<V_IMPORT> vertices;
			std::vector<VN_IMPORT> normals;
			std::vector<VT_IMPORT> texCoords;
			std::vector<F_IMPORT> indices;


			while (std::getline(mod, currentLine))
			{
				if (currentLine[0] != '#' && currentLine != "")
				{
					std::istringstream lineStream(currentLine);
					lineStream >> type;
					if (type == "v")
					{
						V_IMPORT vertex;
						sscanf_s(currentLine.c_str(), "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
						vertices.push_back(vertex);
					}
					else if (m_useTexture && type == "vt")
					{
						VT_IMPORT coord;
						sscanf_s(currentLine.c_str(), "%*s %f %f", &coord.u, &coord.v);
						texCoords.push_back(coord);
					}
					else if (m_useNormals && type == "vn")
					{
						VN_IMPORT normal;
						sscanf_s(currentLine.c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
						normals.push_back(normal);
					}
					else if (type == "f")
					{
						std::vector<I_INPUT> currentIndices;
						I_INPUT in;
						int counter = 0;
						while (!lineStream.eof())
						{
							int i;
							if (lineStream >> i)
							{
								//std::cout << i << std::endl;
								in.arr[counter++] = i;
								if (m_useTexture && m_useNormals && counter > 2)
								{
									currentIndices.push_back(in);
									//std::cout << "----" << std::endl;
									counter = 0;
								}
								else if (m_useTexture && !m_useNormals && counter > 1)
								{
									in.arr[2] = -1;
									currentIndices.push_back(in);
									//std::cout << "----" << std::endl;
									counter = 0;
								}
								else if (!m_useTexture && m_useNormals && counter > 1)
								{
									in.arr[2] = in.arr[1];
									in.arr[1] = -1;
									currentIndices.push_back(in);
									//std::cout << "----" << std::endl;
									counter = 0;
								}
								else if (!m_useTexture && !m_useNormals)
								{
									in.arr[1] = -1;
									in.arr[2] = -1;
									currentIndices.push_back(in);
									//std::cout << "----" << std::endl;
									counter = 0;
								}
							}
							else
							{
								lineStream.clear();
								lineStream.ignore();
							}
						}
						bool isQuad = false;

						if (currentIndices.size() > 3)
						{
							isQuad = true;
						}
						
						if (isQuad)
						{
							F_IMPORT f;

							if (m_clockwise)
							{
								f.vIndex = currentIndices[0].arr[0];
								f.vtIndex = currentIndices[0].arr[1];
								f.vnIndex = currentIndices[0].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[1].arr[0];
								f.vtIndex = currentIndices[1].arr[1];
								f.vnIndex = currentIndices[1].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[2].arr[0];
								f.vtIndex = currentIndices[2].arr[1];
								f.vnIndex = currentIndices[2].arr[2];
								indices.push_back(f);
							}
							else
							{
								f.vIndex = currentIndices[2].arr[0];
								f.vtIndex = currentIndices[2].arr[1];
								f.vnIndex = currentIndices[2].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[1].arr[0];
								f.vtIndex = currentIndices[1].arr[1];
								f.vnIndex = currentIndices[1].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[0].arr[0];
								f.vtIndex = currentIndices[0].arr[1];
								f.vnIndex = currentIndices[0].arr[2];
								indices.push_back(f);

								f.vIndex = currentIndices[0].arr[0];
								f.vtIndex = currentIndices[0].arr[1];
								f.vnIndex = currentIndices[0].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[3].arr[0];
								f.vtIndex = currentIndices[3].arr[1];
								f.vnIndex = currentIndices[3].arr[2];
								indices.push_back(f);
								f.vIndex = currentIndices[2].arr[0];
								f.vtIndex = currentIndices[2].arr[1];
								f.vnIndex = currentIndices[2].arr[2];
								indices.push_back(f);
							}
						}
						else
						{
							F_IMPORT f;
							if (m_clockwise)
							{
								for (int i = 0; i < 3; i++)
								{
									counter = 0;
									f.vIndex = currentIndices[i].arr[counter++];
									f.vtIndex = currentIndices[i].arr[counter++];
									f.vnIndex = currentIndices[i].arr[counter++];
									indices.push_back(f);
								}
							}
							else
							{
								for (int i = 2; i >= 0; i--)
								{
									counter = 0;
									f.vIndex = currentIndices[i].arr[counter++];
									f.vtIndex = currentIndices[i].arr[counter++];
									f.vnIndex = currentIndices[i].arr[counter++];
									indices.push_back(f);
								}
							}
						}
					}
				}
			}
			mod.close();

			m_nrOfVertices = static_cast<int>(indices.size());
			m_vertices = new VERTEX[m_nrOfVertices];
			int indexCounter = 0;
			for (int i = 0; i < m_nrOfVertices; i++)
			{
				VERTEX v;
				int currentIndex = indices[indexCounter++].vIndex - 1;

				v.x = vertices[currentIndex].x;
				v.y = vertices[currentIndex].y;
				v.z = vertices[currentIndex].z;
				if (texCoords.size() > 0)
				{
					v.u = texCoords[currentIndex].u;
					v.v = texCoords[currentIndex].v;
				}
				m_vertices[i] = v;
			}
		}
		else
		{
			std::cout << "failed to load " << path << std::endl;
		}
	}
}

void Model::initExistingModel()
{
	std::vector<VERTEX> vertexVector = TerrainLoader::terrainLoader();
	m_nrOfVertices = vertexVector.size();
	m_vertices = new VERTEX[m_nrOfVertices];

	for (int i = 0; i < m_nrOfVertices; i++)
	{
		m_vertices[i] = vertexVector[i];
	}
}

std::string Model::getName() const
{
	return m_name;
}

VERTEX * Model::getMesh() const
{
	return m_vertices;
}

int Model::getNrOfVertices() const
{
	return m_nrOfVertices;
}
