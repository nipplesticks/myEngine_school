#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "Model.hpp"

class Entity
{
public:
	Entity();
	~Entity();

	void loadModel(Model* m);
	void loadVertexBuffer(ID3D11Device*& device);

	void setRotation(DirectX::XMFLOAT3 rotation);
	void setRotation(float x, float y, float z);
	void rotate(DirectX::XMFLOAT3 rotation);
	void rotate(float x, float y, float z);

	void draw(ID3D11DeviceContext*& deviceContext) const;

	int getNrOfVertices() const;
	ID3D11Buffer* getVertexBuffer() const;

private:
	ID3D11Buffer*		m_vertexBuffer;
	Model*				m_model;
	DirectX::XMVECTOR	m_pos;
	DirectX::XMVECTOR	m_rot;
	DirectX::XMMATRIX	m_worldMatrix;
};

#endif