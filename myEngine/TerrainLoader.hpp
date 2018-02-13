#include <iostream> 
#include <fstream>

namespace TerrainLoader
{
	std::vector<VERTEX> terrainLoader2(std::string path, float normalizeHeight, int width, int height, float normalizeUV, float**& arr)
	{
		//A high anti-sensitivity value means that the colors won't affect the height-values 
		//as much as a low anti-sensitvity value would. 
		int offsetZ = height / 2;
		int offsetX = width / 2;
		int offsetY = 128;

		//TEST
		offsetZ = 0;
		offsetX = 0;

		if (normalizeHeight < 1) normalizeHeight = 1.0f;

		unsigned char **heightMap = new unsigned char*[height];
		float **map = new float*[height];
		for (int i = 0; i < height; i++)
		{
			map[i] = new float[width];
		}
		std::vector<VERTEX> vertexVector;
		std::ifstream heightMapFile;
		heightMapFile.open(path, std::ifstream::binary);
		if (heightMapFile)
		{
			for (int i = 0; i < height; i++)
			{
				unsigned char * buffer = new unsigned char[width];
				heightMapFile.read((char*)buffer, width);
				heightMap[i] = buffer;
			}
			for (int i = 0; i < height; i++)
			{
				for (int k = 0; k < width; k++)
				{
					map[k][i] = (float)heightMap[k][i];
				}
			}

			
			for (int i = 0; i < height; i++)
			{
				for (int k = 0; k < width; k++)
				{
					if (i > 0 && k > 0 && i < (height - 1) && k < (width - 1))
					{
						map[i][k] =
							(map[i - 1][k + 1] + map[i][k + 1] + map[i + 1][k + 1] +
								map[i - 1][k] + map[i][k] + map[i + 1][k] +
								map[i - 1][k - 1] + map[i][k - 1] + map[i + 1][k - 1]) / 9;
					}
				}
			}


			for (int i = 0; i < height - 1; i++)
			{
				for (int j = 0; j < width - 1; j++)
				{
					VERTEX v;

					v = VERTEX{ float(j) - offsetX, float(map[j][i] - offsetY) / normalizeHeight ,float(i) - offsetZ };
					v.u = ((float)j) * normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offsetX, float(map[j][i + 1] - offsetY) / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX,float(map[j + 1][i] - offsetY) / normalizeHeight, float(i) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offsetX, float(map[j][i + 1] - offsetY) / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX, float(map[j + 1][i + 1] - offsetY) / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX, float(map[j + 1][i] - offsetY) / normalizeHeight, float(i) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);
				}
			}

			arr = map;
			heightMapFile.close();

			for (int i = 0; i < height; i++)
			{
				delete[] heightMap[i];
				//delete[] map[i];
			}
			delete[] heightMap;
			//delete[] map;

		}
		return vertexVector;
	}
}

