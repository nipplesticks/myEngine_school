#include "Model.hpp"
#include "TerrainLoader.hpp"
#include <iostream>

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

Model::Model()
{
	m_terrain = nullptr;
	m_nrOfVertices = 0;
	m_name = "";
	m_useTexture = false;
	m_useNormals = false;
	m_texture = nullptr;
	m_sampleState = nullptr;
	m_textureSetting = -1;
	m_terrainStatus = false;
}

Model::~Model()
{
	delete[] m_terrain;
	
	if (m_texture != nullptr) m_texture->Release();
	if (m_sampleState != nullptr) m_sampleState->Release();
	for (Mesh m : m_meshes)
	{
		m.releaseMaterial();
	}
}

void Model::settings(bool texture, bool normals, bool clockwise)
{
	m_useTexture = texture;
	m_useNormals = normals;
	m_clockwise = clockwise;
}

void Model::initModel(std::string path, std::string name, ID3D11Device* device)
{
	m_name = name;

	if (!m_terrainStatus)
	{
		bool firstPas = true;
		std::ifstream mod;
		std::cout << "Importing " << path + name << std::endl;
		mod.open(path + name);
		if (mod)
		{
			std::string currentLine = "";
			std::string type = "";
			std::vector<V_IMPORT> vertices;
			std::vector<VN_IMPORT> normals;
			std::vector<VT_IMPORT> texCoords;
			std::vector<F_IMPORT> indices;

			std::string mtllib_name = "";

			while (std::getline(mod, currentLine))
			{
				if (currentLine[0] != '#' && currentLine != "")
				{
					std::istringstream lineStream(currentLine);
					lineStream >> type;
					if (type == "mtllib" && m_useTexture)
					{
						mtllib_name = read_mtllib(currentLine);
					}
					else if (type == "usemtl" && m_useTexture)
					{
						if (firstPas)
						{
							firstPas = false;
						}
						else
						{
							loadVertexVector(vertices, normals, texCoords, indices);
							vertices.clear();
							normals.clear();
							texCoords.clear();
							indices.clear();
						}
						import_material(path, mtllib_name, currentLine, device);

					}
					else if (type == "g")
					{
						std::string meshName = "";
						lineStream >> meshName;
						std::cout << "Loading " << meshName << std::endl;
					}
					else if (type == "v")
						read_vertex(currentLine, vertices);
					else if (type == "vt" && m_useTexture)
						read_vertexTexture(currentLine, texCoords);
					else if (type == "vn" && m_useNormals)
						read_vertexNormal(currentLine, normals);
					else if (type == "f")
						read_faces(lineStream, indices);
				}
			}
			mod.close();

			loadVertexVector(vertices, normals, texCoords, indices);

		/*	m_nrOfVertices = static_cast<int>(indices.size());
			m_terrain = new VERTEX[m_nrOfVertices];
			int indexCounter = 0;

			for (int i = 0; i < m_nrOfVertices; i++)
			{
				VERTEX v;
				int currentVIndex = indices[indexCounter].vIndex - 1;

				v.x = vertices[currentVIndex].x;
				v.y = vertices[currentVIndex].y;
				v.z = vertices[currentVIndex].z;
				if (texCoords.size() > 0)
				{
					int currentTXIndex = indices[indexCounter].vtIndex - 1;
					v.u = texCoords[currentTXIndex].u;
					v.v = texCoords[currentTXIndex].v;
				}
				if (normals.size() > 0)
				{
					int currentNIndex = indices[indexCounter].vnIndex - 1;
					v.xN = normals[currentNIndex].x;
					v.yN = normals[currentNIndex].y;
					v.zN = normals[currentNIndex].z;
				}

				m_terrain[i] = v;
				indexCounter++;
			}*/
		}
		else
		{
			std::cout << "failed to load " << path << std::endl;
		}
	}
}

void Model::initTexture(wchar_t* path, ID3D11Device*& device, int textureSetting)
{
	std::wstring ws(path);
	std::string str(ws.begin(), ws.end());
	std::cout << "Importing " << str << std::endl;

	ID3D11Resource* pTexture = nullptr;
	HRESULT hr;
	if (str.find(".dds") != -1 || str.find(".DDS") != -1)
		hr = DirectX::CreateDDSTextureFromFile(device, path, &pTexture, &m_texture);
	else
	{
		hr = DirectX::CreateWICTextureFromFile(device, path, &pTexture, &m_texture);
	}

	if (FAILED(hr))
	{
		std::cout << "FAILED" << std::endl;
	}
	else
	{
		m_textureSetting = textureSetting;
	}


	pTexture->Release();
}

ID3D11ShaderResourceView * Model::getTextureResourceView() const
{
	return m_texture;
}

ID3D11SamplerState * Model::getSampleState() const
{
	return m_sampleState;
}

int Model::getTextureSetting() const
{
	return m_textureSetting;
}

void Model::initTerrainViaHeightMap(std::string path, std::string name, float normalizeHeight, int width, int height, float normalizeUV, float**& arr)
{
	m_name = name;
	m_terrainStatus = true;
	std::cout << "Importing " << path << std::endl;
	std::vector<VERTEX> vertexVector = TerrainLoader::terrainLoader2(path, normalizeHeight, width, height, normalizeUV, arr);
	m_nrOfVertices = static_cast<int>(vertexVector.size());
	m_terrain = new VERTEX[m_nrOfVertices];

	for (int i = 0; i < m_nrOfVertices; i++)
	{
		m_terrain[i] = vertexVector[i];
	}
}

bool Model::isTerrain() const
{
	return m_terrainStatus;
}

const std::vector<Mesh>& Model::getMeshes() const
{
	return m_meshes;
}

std::string Model::getName() const
{
	return m_name;
}

VERTEX * Model::getTerrainMesh() const
{
	return m_terrain;
}

int Model::getNrOfVertices() const
{
	return m_nrOfVertices;
}

std::string Model::read_mtllib(const std::string & mtllib)
{
	size_t i = mtllib.find_last_of(".mtl");
	if (i == -1)
		i = mtllib.find_last_of(".MTL");
	if (i == -1)
		return "NOFILE";

	size_t k = i;
	while (mtllib[k] != '/' && mtllib[k] != '\\' && mtllib[k] != ' ')
	{
		k--;
	}

	std::string fileName = mtllib.substr(k + 1, i + 3);

	return fileName;
}

void Model::read_vertex(const std::string & vertex, std::vector<V_IMPORT>& vertices)
{
	V_IMPORT v;
	sscanf_s(vertex.c_str(), "%*s %f %f %f", &v.x, &v.y, &v.z);
	vertices.push_back(v);
}

void Model::read_vertexTexture(const std::string & vertexTexture, std::vector<VT_IMPORT>& vertexTextures)
{
	VT_IMPORT coord;
	sscanf_s(vertexTexture.c_str(), "%*s %f %f", &coord.u, &coord.v);
	vertexTextures.push_back(coord);
}

void Model::read_vertexNormal(const std::string & vertexNormal, std::vector<VN_IMPORT>& vertexNormals)
{
	VN_IMPORT normal;
	sscanf_s(vertexNormal.c_str(), "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
	vertexNormals.push_back(normal);
}

void Model::read_faces(std::istringstream & face, std::vector<F_IMPORT>& faces)
{
	std::vector<I_INPUT> currentIndices;
	I_INPUT in;
	int counter = 0;
	while (!face.eof())
	{
		int i;
		if (face >> i)
		{
			in.arr[counter++] = i;
			if (m_useTexture && m_useNormals && counter > 2)
			{
				currentIndices.push_back(in);
				counter = 0;
			}
			else if (m_useTexture && !m_useNormals && counter > 1)
			{
				in.arr[2] = -1;
				currentIndices.push_back(in);
				counter = 0;
			}
			else if (!m_useTexture && m_useNormals && counter > 1)
			{
				in.arr[2] = in.arr[1];
				in.arr[1] = -1;
				currentIndices.push_back(in);
				counter = 0;
			}
			else if (!m_useTexture && !m_useNormals)
			{
				in.arr[1] = -1;
				in.arr[2] = -1;
				currentIndices.push_back(in);
				counter = 0;
			}
		}
		else
		{
			face.clear();
			face.ignore();
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
			faces.push_back(f);
			f.vIndex = currentIndices[1].arr[0];
			f.vtIndex = currentIndices[1].arr[1];
			f.vnIndex = currentIndices[1].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[2].arr[0];
			f.vtIndex = currentIndices[2].arr[1];
			f.vnIndex = currentIndices[2].arr[2];
			faces.push_back(f);

			f.vIndex = currentIndices[2].arr[0];
			f.vtIndex = currentIndices[2].arr[1];
			f.vnIndex = currentIndices[2].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[3].arr[0];
			f.vtIndex = currentIndices[3].arr[1];
			f.vnIndex = currentIndices[3].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[0].arr[0];
			f.vtIndex = currentIndices[0].arr[1];
			f.vnIndex = currentIndices[0].arr[2];
			faces.push_back(f);
		}
		else
		{
			f.vIndex = currentIndices[2].arr[0];
			f.vtIndex = currentIndices[2].arr[1];
			f.vnIndex = currentIndices[2].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[1].arr[0];
			f.vtIndex = currentIndices[1].arr[1];
			f.vnIndex = currentIndices[1].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[0].arr[0];
			f.vtIndex = currentIndices[0].arr[1];
			f.vnIndex = currentIndices[0].arr[2];
			faces.push_back(f);

			f.vIndex = currentIndices[0].arr[0];
			f.vtIndex = currentIndices[0].arr[1];
			f.vnIndex = currentIndices[0].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[3].arr[0];
			f.vtIndex = currentIndices[3].arr[1];
			f.vnIndex = currentIndices[3].arr[2];
			faces.push_back(f);
			f.vIndex = currentIndices[2].arr[0];
			f.vtIndex = currentIndices[2].arr[1];
			f.vnIndex = currentIndices[2].arr[2];
			faces.push_back(f);
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
				faces.push_back(f);
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
				faces.push_back(f);
			}
		}
	}
}

std::string Model::get_textureFile(const std::string & mtllib, const std::string & material)
{
	std::ifstream file;
	file.open(mtllib);
	std::string mat = "";
	bool found = false;
	if (file)
	{
		std::string buffer = "";
		std::string search = "";
		std::istringstream searchStream(material);
		searchStream >> buffer;
		searchStream >> search;

		while (!file.eof() && std::getline(file, mat) && !found)
		{
			std::istringstream stream(mat);
			stream >> buffer;

			if (buffer == "newmtl")
			{
				stream >> mat;

				if (mat == search)
				{
					std::getline(file, mat);
					std::istringstream theMaterial(mat);
					theMaterial >> buffer;
					theMaterial >> mat;

					return mat;
				}
			}
		}
	}
	else
	{
		return "NOFILE";
	}
	file.close();

	return "NOFILE";
}

void Model::import_material(const std::string & path, const std::string & mtllib, const std::string & material, ID3D11Device * device)
{
	std::ifstream file;
	file.open(path + mtllib);
	if (file)
	{
		std::string mat = "";
		bool done = false;
		std::string buffer = "";
		std::string search = "";
		std::istringstream searchStream(material);
		searchStream >> buffer;
		searchStream >> search;

		while (!file.eof() && std::getline(file, mat) && !done)
		{
			if (mat[0] != NULL && mat[0] != '#')
			{
				std::istringstream stream(mat);
				stream >> buffer;

				if (buffer == "newmtl")
				{
					stream >> mat;

					if (mat == search)
					{
						done = true;
						bool materialImported = false;
						std::string currentLine;
						std::string type;
						Material aMaterial;
						aMaterial.setMaterialName(mat);
						while (!file.eof() && std::getline(file, currentLine) && !materialImported)
						{
							if (currentLine[0] != NULL && currentLine[0] != '#')
							{
								std::istringstream stream(currentLine);
								stream >> type;

								if (type == "Ka")
								{
									DirectX::XMFLOAT3 ka;
									sscanf_s(currentLine.c_str(), "%*s %f %f %f", &ka.x, &ka.y, &ka.z);
									aMaterial.setAmbientLevel(ka);
								}
								else if (type == "Kd")
								{
									DirectX::XMFLOAT3 kd;
									sscanf_s(currentLine.c_str(), "%*s %f %f %f", &kd.x, &kd.y, &kd.z);
									aMaterial.setDiffuseLevel(kd);
								}
								else if (type == "Ks")
								{
									DirectX::XMFLOAT3 ks;
									sscanf_s(currentLine.c_str(), "%*s %f %f %f", &ks.x, &ks.y, &ks.z);
									aMaterial.setSpecularLevel(ks);
								}
								else if (type == "Ke")
								{
									DirectX::XMFLOAT3 ke;
									sscanf_s(currentLine.c_str(), "%*s %f %f %f", &ke.x, &ke.y, &ke.z);
									aMaterial.setEmissiveColor(ke);
								}
								else if (type == "Ns")
								{
									float ns;
									sscanf_s(currentLine.c_str(), "%*s %f", &ns);
									aMaterial.setSpecularRange(ns);
								}
								else if (type == "d")
								{
									float d;
									sscanf_s(currentLine.c_str(), "%*s %f", &d);
									aMaterial.setDissolveLevel(d);

								}
								else if (type == "Tr")
								{
									float tr;
									sscanf_s(currentLine.c_str(), "%*s %f", &tr);
									aMaterial.setDissolveLevel(1.0f - tr);
								}
								else if (type == "Ni")
								{
									float ni;
									sscanf_s(currentLine.c_str(), "%*s %f", &ni);
									aMaterial.setOpticalDensity(ni);
								}
								else if (type == "sharpness")
								{
									float sharp;
									sscanf_s(currentLine.c_str(), "%*s %f", &sharp);
									aMaterial.setSharpness(sharp);
								}
								else if (type == "illum")
								{
									int illum;
									sscanf_s(currentLine.c_str(), "%*s %i", &illum);
									aMaterial.setIllum(illum);
								}
								else if (type == "map_Ka")
								{
									std::string texturePath = "";
									stream >> texturePath;
							
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setAmbientTexture(rw, r);

								}
								else if (type == "map_Kd")
								{
									std::string texturePath = "";
									stream >> texturePath;
									
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setDiffuseTexture(rw, r);
								}
								else if (type == "map_Ks")
								{
									std::string texturePath = "";
									stream >> texturePath;
									
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setSpecularColorTexture(rw, r);
								}
								else if (type == "map_Ke")
								{
									std::string texturePath = "";
									stream >> texturePath;
									
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setEmessiveTextureMap(rw, r);
								}
								else if (type == "map_Ns")
								{
									std::string texturePath = "";
									stream >> texturePath;
									
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setSpecularHighlightComponent(rw, r);
								}
								else if (type == "map_d")
								{
									std::string texturePath = "";
									stream >> texturePath;
									
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setAlphaMap(rw, r);
								}
								else if (type == "map_Bump" || type == "Bump")
								{
									std::string texturePath = "";
									stream >> texturePath;
								
									ID3D11Resource* r;
									ID3D11ShaderResourceView* rw;

									import_texture(path + texturePath, device, r, rw);

									aMaterial.setBumpMap(rw, r);
								}
								else if (type == "refl")
								{

								}
								else if (type == "disp")
								{

								}
								else if (type == "decal")
								{

								}
								else if (type == "newmtl")
								{
									materialImported = true;
								}
							}
						}
						m_tempMaterial = aMaterial;
					}
				}
			}
		}
	}
	file.close();
}

void Model::import_texture(const std::string & path, ID3D11Device * device, ID3D11Resource *& texture, ID3D11ShaderResourceView *& resource)
{
	if (device)
	{
		std::wstring ws = std::wstring(path.begin(), path.end());
		const wchar_t* tex = ws.c_str();
		HRESULT hr;
		if (path.find(".dds") == -1 && path.find(".DDS") == -1)
			hr = DirectX::CreateWICTextureFromFile(device, tex, &texture, &resource);
		else
			hr = DirectX::CreateDDSTextureFromFile(device, tex, &texture, &resource);
	}
}


void Model::loadVertexVector(const std::vector<V_IMPORT>& vertices, const std::vector<VN_IMPORT>& normals, const std::vector<VT_IMPORT>& texCoords, const std::vector<F_IMPORT>& indices)
{
	std::vector<VERTEX> t_vert;
	int indexCounter = 0;

	for (unsigned int i = 0; i < indices.size(); i++)
	{
		VERTEX v;
		int currentVIndex = indices[indexCounter].vIndex - 1;

		if (currentVIndex < 0)
			currentVIndex *= -1;

		v.x = vertices[currentVIndex].x;
		v.y = vertices[currentVIndex].y;
		v.z = vertices[currentVIndex].z;
		if (texCoords.size() > 0)
		{
			int currentTXIndex = indices[indexCounter].vtIndex - 1;
			if (currentTXIndex < 0)
				currentTXIndex *= -1;
			v.u = texCoords[currentTXIndex].u;
			v.v = texCoords[currentTXIndex].v;
		}
		if (normals.size() > 0)
		{
			int currentNIndex = indices[indexCounter].vnIndex - 1;
			if (currentNIndex < 0)
				currentNIndex *= -1;
			v.xN = normals[currentNIndex].x;
			v.yN = normals[currentNIndex].y;
			v.zN = normals[currentNIndex].z;
		}

		t_vert.push_back(v);
		indexCounter++;
	}
	Mesh m;
	m.setVertices(t_vert);
	m.setMaterial(m_tempMaterial);
	m.buildtangentSpace();
	m_meshes.push_back(m);
}
