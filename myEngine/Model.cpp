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
	
	if (m_texture != nullptr) m_texture->Release();
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

void Model::initTexture(std::string path, ID3D11Device*& device)
{
	std::cout << "Importing " << path << std::endl;

	unsigned char header[54];			// The header of the BMP image
	unsigned int dataPos = 0;			// The actual position of the data in the BMP image
	unsigned int width = 0;				// Width of the BMP image, duh
	unsigned int height = 0;			// Height of the BMP image, duh
	unsigned int imageSize = 0;			// width * height * 3 (RGB == 3)

	unsigned char* data = nullptr;		// RGB data

	FILE* img;

	fopen_s(&img, path.c_str(), "rb");	//read bytes
	
	if (img &&
		fread(header, 1, 54, img) == 54 &&
		header[0] == 'B' &&
		header[1] == 'M')
	{
		dataPos = *(int*)&(header[0x0A]);
		imageSize = *(int*)&(header[0x22]);
		width = *(int*)&(header[0x12]);
		height = *(int*)&(header[0x16]);

		// In case of bad BMP
		if (imageSize == 0) imageSize = width * height * 3;
		if (dataPos == 0) dataPos = 54;

		data = new unsigned char[imageSize];
		fread(data, 1, imageSize, img);
		fclose(img);

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = data;
		initData.SysMemPitch = static_cast<UINT>(width * 3 * sizeof(char));
		initData.SysMemSlicePitch = static_cast<UINT>(imageSize * 3);

		ID3D11Texture2D* tex = nullptr;
		HRESULT hr = device->CreateTexture2D(&desc, &initData, &tex);
		if (SUCCEEDED(hr) && tex != 0)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			memset(&SRVDesc, 0, sizeof(SRVDesc));
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = 1;
			SRVDesc.Texture2D.MostDetailedMip = 0;
			hr = device->CreateShaderResourceView(tex, &SRVDesc, &m_texture);

			if (FAILED(hr))
			{
				std::cout << "Failed to create texture\n";
			}
			tex->Release();
		}
		else
		{
			std::cout << "failed to create texture\n";
		}

	}
	else
	{
		std::cout << "Error reading file\n";
	}

}

ID3D11ShaderResourceView * Model::getTextureResourceView() const
{
	return m_texture;
}

void Model::initExistingModel(std::string path)
{
	std::cout << "Importing " << path << std::endl;
	std::vector<VERTEX> vertexVector = TerrainLoader::terrainLoader(path);
	m_nrOfVertices = static_cast<int>(vertexVector.size());
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
