#include <iostream> 
#include <fstream>

namespace TerrainLoader
{
	std::vector<VERTEX> terrainLoader(std::string path)
	{
		//A high anti-sensitivity value means that the colors won't affect the height-values 
		//as much as a low anti-sensitvity value would. 
		int anti_sensitivity = 255; 
		const int size = 100;

		unsigned char heightMap[size][size];
	    std::vector<VERTEX> vertexVector;
		std::ifstream heightMapFile;
		heightMapFile.open(path, std::ifstream::binary);

		if (heightMapFile.is_open())
		{
			for (int i = 0; i < size; i++)
			{
				heightMapFile.read((char*)heightMap[i], size);
			}

			/*std::cout << "Values loaded to heightMap: " << std::endl;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					std::cout << (int)heightMap[j][i] << std::endl; 
				}
			}*/


			for (int i = 0; i < size - 1; i++)
			{
				for (int j = 0; j < size - 1; j++)
				{
					VERTEX v;

					v = VERTEX{float(j),(float(heightMap[j][i] - 128) / anti_sensitivity),float(i)};
					vertexVector.push_back(v);
					
					v = VERTEX{float(j),(float(heightMap[j][i + 1] - 128) / anti_sensitivity),float(i + 1)};
					vertexVector.push_back(v);

					v = VERTEX{float(j + 1),(float(heightMap[j + 1][i] - 128) / anti_sensitivity),float(i)};
					vertexVector.push_back(v);

					v = VERTEX{float(j),(float(heightMap[j][i + 1] - 128) / anti_sensitivity),float(i + 1)};
					vertexVector.push_back(v);

					v = VERTEX{float(j + 1),(float(heightMap[j + 1][i + 1] - 128) / anti_sensitivity),float(i + 1)};
					vertexVector.push_back(v);

					v = VERTEX{float(j + 1),(float(heightMap[j + 1][i] - 128) / anti_sensitivity),float(i)};
					vertexVector.push_back(v);
				}
			}
			heightMapFile.close();

			/*
			for (int i = 0; i < vertexVector.size(); i++)
			{
				std::cout << vertexVector[i].y << ", ";
				if (i % size == 0 && i != 0)
				{
					std::cout << "\n";
				}
			}
			*/
		
		}
		return vertexVector;
	}
}
