#include <iostream> 
#include <fstream>

namespace TerrainLoader
{
	std::vector<VERTEX> terrainLoader()
	{
		//A high anti-sensitivity value means that the colors won't affect the height-values 
		//as much as a low anti-sensitvity value would. 
		int anti_sensitivity = 255; 
		
		unsigned char heightMap[100][100];
	    std::vector<VERTEX> vertexVector;
		std::ifstream heightMapFile;
		heightMapFile.open("HeightMap/Test5.data", std::ifstream::binary);

		if (heightMapFile.is_open())
		{
			for (int i = 0; i < 100; i++)
			{
				heightMapFile.read((char*)heightMap[i], 100);
			}

			/*std::cout << "Values loaded to heightMap: " << std::endl;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					std::cout << (int)heightMap[j][i] << std::endl; 
				}
			}*/


			for (int i = 0; i < 100 - 1; i++)
			{
				for (int j = 0; j < 100 - 1; j++)
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

			/*std::cout << "Before pass: " << std::endl; 
			for (int i = 0; i < vertexVector.size(); i++)
			{
				std::cout << vertexVector.at(i).y << std::endl;
			}*/
			return vertexVector;
		}
	}
}
