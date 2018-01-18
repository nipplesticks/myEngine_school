#include "Entity.hpp"
using namespace DirectX;

Entity::Entity()
{
	init();

	//REMOVE THIS LATER
	m_model = new Model;
	m_model->initModel("");
}

Entity::~Entity()
{
	m_vertexBuffer->Release();
	m_constantBuffer->Release();

	//REMOVE THIS LATER
	delete m_model;
}

void Entity::loadModel(Model * m)
{
	m_model = m;
}

void Entity::loadBuffers(ID3D11Device*& device)
{
	// Vertex Buffer
	D3D11_BUFFER_DESC vBufferDesc;
	memset(&vBufferDesc, 0, sizeof(vBufferDesc));
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(VERTEX) * m_model->getNrOfVertices();

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = m_model->getMesh();
	HRESULT hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vertexBuffer);

	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	hr = device->CreateBuffer(&cBufferDesc, nullptr, &m_constantBuffer);
}

void Entity::setRotation(DirectX::XMFLOAT3 rotation, float angle)
{
	m_rot = rotation;
	m_currentAngle = XMConvertToRadians(angle);
	buildMatrix();
}

void Entity::setRotation(float x, float y, float z, float angle)
{
	XMFLOAT3 rot(x, y, z);
	setRotation(rot, angle);
}

void Entity::rotate(DirectX::XMFLOAT3 rotation, float angle)
{
	m_rot = rotation;
	m_currentAngle += XMConvertToRadians(angle);
	buildMatrix();
}

void Entity::rotate(float x, float y, float z, float angle)
{
	XMFLOAT3 rot(x, y, z);
	rotate(rot, angle);
}

void Entity::setPosition(DirectX::XMFLOAT3 position)
{
	m_pos = position;
	buildMatrix();
}

void Entity::setPosition(float x, float y, float z)
{
	XMFLOAT3 pos(x, y, z);
	setPosition(pos);
}

void Entity::move(DirectX::XMFLOAT3 position)
{
	m_pos = add(m_pos, position);
	buildMatrix();
}

void Entity::move(float x, float y, float z)
{
	XMFLOAT3 pos(x, y, z);
	move(pos);
}

void Entity::setScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	buildMatrix();
}

void Entity::setScale(float x, float y, float z)
{
	XMFLOAT3 scale(x, y, z);
	setScale(scale);
}

void Entity::scale(DirectX::XMFLOAT3 scale)
{
	m_scale = add(m_scale, scale);
	buildMatrix();
}

void Entity::scale(float x, float y, float z)
{
	XMFLOAT3 scale(x, y, z);
	this->scale(scale);
}

void Entity::setProjectionMatrix(DirectX::XMMATRIX projection)
{
	m_projectionMatrix = projection;
}

void Entity::cameraMoved(DirectX::XMMATRIX view)
{
	m_viewMatrix = view;
	buildMatrix();
}

void Entity::draw(ID3D11DeviceContext *& deviceContext) const
{
	UINT32 vertexSize = sizeof(float) * 5;
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &vertexSize, &offset);
	///m_DeviceContext->PSSetShaderResources(0, 1, &m_BthTextureView);
	
	D3D11_MAPPED_SUBRESOURCE dataPtr;
	deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	//	Copy memory from CPU to GPU
	memcpy(dataPtr.pData, &m_cBuffer, sizeof(CONSTANT_BUFFER));

	// Unmap constant buffer so that we can use it again in the GPU
	deviceContext->Unmap(m_constantBuffer, 0);
	// set resources to shaders
	deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->GSSetConstantBuffers(0, 1, &m_constantBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	deviceContext->Draw(m_model->getNrOfVertices(), 0);
}

XMFLOAT3 Entity::add(XMFLOAT3 tar, XMFLOAT3 adder) const
{
	XMFLOAT3 result;
	result.x = tar.x + adder.x;
	result.y = tar.y + adder.y;
	result.z = tar.z + adder.z;
	return result;
}

void Entity::buildMatrix()
{
	XMMATRIX translate = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
	XMVECTOR axis = XMLoadFloat3(&m_rot);
	XMMATRIX rotation = XMMatrixRotationAxis(axis, m_currentAngle);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	m_worldMatrix = rotation * scale * translate;
	//m_worldMatrix = scale * translate * rotation;

	m_cBuffer.WorldMatrix = XMMatrixTranspose(m_worldMatrix);
	m_cBuffer.WVPMatrix = XMMatrixTranspose(m_worldMatrix * m_viewMatrix * m_projectionMatrix);
}

void Entity::init()
{
	m_vertexBuffer = nullptr;
	m_constantBuffer = nullptr;
	m_model = nullptr;

	m_rot = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_currentAngle = 0.0f;
	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

int Entity::getNrOfVertices() const
{
	return m_model->getNrOfVertices();
}

ID3D11Buffer * Entity::getVertexBuffer() const
{
	return m_vertexBuffer;
}

