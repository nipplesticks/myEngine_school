#include "Entity.hpp"
using namespace DirectX;

Entity::Entity()
{
	m_vertexBuffer = nullptr;
	m_model = nullptr;

	//REMOVE THIS LATER
	m_model = new Model;
	m_model->initModel("");
}

Entity::~Entity()
{
	m_vertexBuffer->Release();

	//REMOVE THIS LATER
	delete m_model;
}

void Entity::loadModel(Model * m)
{
	m_model = m;
}

void Entity::loadVertexBuffer(ID3D11Device*& device)
{
	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(m_model->getMesh());

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_model->getMesh();
	device->CreateBuffer(&bufferDesc, &data, &m_vertexBuffer);
}

void Entity::setRotation(DirectX::XMFLOAT3 rotation)
{
	m_rot = XMLoadFloat3(&rotation);
}

void Entity::setRotation(float x, float y, float z)
{
	XMFLOAT3 rot(x, y, z);
	setRotation(rot);
}

void Entity::rotate(DirectX::XMFLOAT3 rotation)
{
	XMVECTOR rot = XMLoadFloat3(&rotation);
	m_rot += rot;
}

void Entity::rotate(float x, float y, float z)
{
	XMFLOAT3 rot(x, y, z);
	rotate(rot);
}

int Entity::getNrOfVetices() const
{
	return m_model->getNrOfVertices();
}

ID3D11Buffer * Entity::getVertexBuffer() const
{
	return m_vertexBuffer;
}

