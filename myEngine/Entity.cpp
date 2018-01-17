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
	bufferDesc.ByteWidth = sizeof(VERTEX) * m_model->getNrOfVertices();

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_model->getMesh();
	
	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &m_vertexBuffer);
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

void Entity::draw(ID3D11DeviceContext *& deviceContext) const
{
	UINT32 vertexSize = sizeof(float) * 5;
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &vertexSize, &offset);
	///m_DeviceContext->PSSetShaderResources(0, 1, &m_BthTextureView);
	/*
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	m_DeviceContext->Map(m_ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	//	Copy memory from CPU to GPU
	///memcpy(dataptr.pData, &THE_CONSTANT_BUFFER_IN_USE, sizeof(CONSTANT_BUFFER));

	// Unmap constant buffer so that we can use it again in the GPU
	m_DeviceContext->Unmap(m_ConstantBuffer, 0);
	// set resources to shaders
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_DeviceContext->GSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	m_DeviceContext->PSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	*/
	deviceContext->Draw(m_model->getNrOfVertices(), 0);
}

int Entity::getNrOfVertices() const
{
	return m_model->getNrOfVertices();
}

ID3D11Buffer * Entity::getVertexBuffer() const
{
	return m_vertexBuffer;
}

