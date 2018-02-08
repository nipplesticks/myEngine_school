#ifndef MODEL_HPP
#define MODEL_HPP
#include "Mesh.hpp"
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
	void initModel(std::string path, std::string name, ID3D11Device* device = nullptr);
	void initTexture(wchar_t* path, ID3D11Device*& device, int textureSetting = 0);
	ID3D11ShaderResourceView* getTextureResourceView() const;
	ID3D11SamplerState* getSampleState() const;
	int getTextureSetting() const;
	void initTerrainViaHeightMap(std::string path, std::string name, float normalizeHeight, int width, int height, float normalizeUV, float**& arr);
	bool isTerrain() const;

	const std::vector<Mesh>& getMeshes() const;

	std::string getName() const;
	VERTEX* getTerrainMesh() const;

	int getNrOfVertices() const;

private:
	std::string read_mtllib(const std::string &mtllib);
	void		read_vertex(const std::string &vertex, std::vector<V_IMPORT> &vertices);
	void		read_vertexTexture(const std::string &vertexTexture, std::vector<VT_IMPORT> &vertexTextures);
	void		read_vertexNormal(const std::string &vertexNormal, std::vector<VN_IMPORT> &vertexNormals);
	void		read_faces(std::istringstream &face, std::vector<F_IMPORT> &faces);
	std::string get_textureFile(const std::string & mtllib, const std::string & material);
	void		import_material(const std::string &path, const std::string &mtllib, const std::string &material, ID3D11Device* device);
	
	
	void import_texture(
		const std::string &path,
		ID3D11Device* device,
		ID3D11Resource*& texture,
		ID3D11ShaderResourceView*& resource
	);
	
	
	void		loadVertexVector(const std::vector<V_IMPORT> &vertices,
		const std::vector<VN_IMPORT> &normals, const std::vector<VT_IMPORT> &texCoords,
		const std::vector<F_IMPORT> &indices);
private:
	std::vector<Mesh> m_meshes;
	Material m_tempMaterial;

	VERTEX* m_terrain;
	ID3D11ShaderResourceView*	m_texture;
	ID3D11SamplerState*			m_sampleState;

	int m_textureSetting;
	int m_nrOfVertices;
	std::string m_name;
	bool m_useTexture;
	bool m_useNormals;
	bool m_clockwise;
	bool m_terrainStatus;
};
#endif