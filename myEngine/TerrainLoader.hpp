#include <iostream> 
#include <fstream>

namespace TerrainLoader
{
	std::vector<VERTEX> terrainLoader(std::string path)
	{
		//A high anti-sensitivity value means that the colors won't affect the height-values 
		//as much as a low anti-sensitvity value would. 
		int anti_sensitivity = 15; 
		const int size = 800;
		int offset = size / 2;
		float textureSize = 0.15f;

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

			for (int i = 0; i < size; i++)
			{
				for (int k = 0; k < size; k++)
				{
					if (i > 0 && k > 0 && i < (size - 1) && k < (size - 1))
					{
						heightMap[i][k] =
							(heightMap[i - 1][k + 1] + heightMap[i][k + 1] + heightMap[i + 1][k + 1] +
								heightMap[i - 1][k] + heightMap[i][k] + heightMap[i + 1][k] +
								heightMap[i - 1][k - 1] + heightMap[i][k - 1] + heightMap[i + 1][k - 1]) / 9;
					}
				}
			}
			for (int i = 0; i < size - 1; i++)
			{
				for (int j = 0; j < size - 1; j++)
				{
					VERTEX v;
					
					v = VERTEX{ float(j) - offset,(float(heightMap[j][i] - 128) / anti_sensitivity),float(i) - offset };
					v.u = ((float)j) * textureSize;
					v.v = ((float)i)* textureSize;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offset,(float(heightMap[j][i + 1] - 128) / anti_sensitivity),float(i + 1) - offset };
					v.u = ((float)j)* textureSize;
					v.v = ((float)i + 1)* textureSize;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offset,(float(heightMap[j + 1][i] - 128) / anti_sensitivity),float(i) - offset };
					v.u = ((float)j + 1)* textureSize;
					v.v = ((float)i)* textureSize;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offset,(float(heightMap[j][i + 1] - 128) / anti_sensitivity),float(i + 1) - offset };
					v.u = ((float)j)* textureSize;
					v.v = ((float)i + 1)* textureSize;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offset,(float(heightMap[j + 1][i + 1] - 128) / anti_sensitivity),float(i + 1) - offset };
					v.u = ((float)j + 1)* textureSize;
					v.v = ((float)i + 1)* textureSize;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offset,(float(heightMap[j + 1][i] - 128) / anti_sensitivity),float(i) - offset };
					v.u = ((float)j + 1)* textureSize;
					v.v = ((float)i)* textureSize;
					vertexVector.push_back(v);
				}
			}
			heightMapFile.close();
		
		}
		return vertexVector;
	}

	std::vector<VERTEX> terrainLoader2(std::string path, float normalizeHeight, int width, int height, float normalizeUV)
	{
		//A high anti-sensitivity value means that the colors won't affect the height-values 
		//as much as a low anti-sensitvity value would. 
		int offsetZ = height / 2;
		int offsetX = width / 2;

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
					map[k][i] = map[k][i] / normalizeHeight;
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

					v = VERTEX{ float(j) - offsetX, map[j][i] - 128 / normalizeHeight ,float(i) - offsetZ };
					v.u = ((float)j) * normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offsetX, map[j][i + 1] - 128 / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX,map[j + 1][i] - 128 / normalizeHeight, float(i) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j) - offsetX, map[j][i + 1] - 128 / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX, map[j + 1][i + 1] - 128 / normalizeHeight, float(i + 1) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i + 1)* normalizeUV;
					vertexVector.push_back(v);

					v = VERTEX{ float(j + 1) - offsetX, map[j + 1][i] - 128 / normalizeHeight, float(i) - offsetZ };
					v.u = ((float)j + 1)* normalizeUV;
					v.v = ((float)i)* normalizeUV;
					vertexVector.push_back(v);
				}
			}
			heightMapFile.close();

			for (int i = 0; i < height; i++)
			{
				delete[] heightMap[i];
				delete[] map[i];
			}
			delete[] heightMap;
			delete[] map;

		}
		return vertexVector;
	}
}

