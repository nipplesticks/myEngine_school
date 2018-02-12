#include "Entity.hpp"
using namespace DirectX;

Entity::Entity()
{
	init();
}

Entity::~Entity()
{
	if (m_vertexBuffer != nullptr)
		m_vertexBuffer->Release();
	if (m_constantBuffer != nullptr)
		m_constantBuffer->Release();
}

void Entity::loadModel(Model * m)
{
	m_model = m;
}

void Entity::loadBuffers(ID3D11Device*& device)
{
	if (!m_model->isTerrain())
	{
		const std::vector<Mesh> mesh = m_model->getMeshes();
		size_t nrOfMeshes = mesh.size();
	
		for (size_t i = 0; i < nrOfMeshes; i++)
		{
			// Vertex Buffer
			D3D11_BUFFER_DESC vBufferDesc;
			memset(&vBufferDesc, 0, sizeof(vBufferDesc));
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			//vBufferDesc.ByteWidth = sizeof(VERTEX) * m_model->getNrOfVertices();
			vBufferDesc.ByteWidth = sizeof(VERTEX) * static_cast<UINT>(mesh[i].getVertices().size());

			ID3D11Buffer* pBuffer = nullptr;
			D3D11_SUBRESOURCE_DATA vData;
			vData.pSysMem = m_model->getMeshes()[i].getVertices().data();
			//HRESULT hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vertexBuffer);
			HRESULT hr = device->CreateBuffer(&vBufferDesc, &vData, &pBuffer);
			m_vertexBufferVector.push_back(pBuffer);
		}
	}
	else
	{
		// Vertex Buffer
		D3D11_BUFFER_DESC vBufferDesc;
		memset(&vBufferDesc, 0, sizeof(vBufferDesc));
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//vBufferDesc.ByteWidth = sizeof(VERTEX) * m_model->getNrOfVertices();
		vBufferDesc.ByteWidth = sizeof(VERTEX) * static_cast<UINT>(m_model->getNrOfVertices());

		ID3D11Buffer* pBuffer = nullptr;
		D3D11_SUBRESOURCE_DATA vData;
		vData.pSysMem = m_model->getTerrainMesh();
		//HRESULT hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vertexBuffer);
		HRESULT hr = device->CreateBuffer(&vBufferDesc, &vData, &pBuffer);
		m_vertexBufferVector.push_back(pBuffer);
	}

	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = device->CreateBuffer(&cBufferDesc, nullptr, &m_constantBuffer);
}

void Entity::bindVertexShader(ID3D11VertexShader * vertexShader)
{
	m_vertexShader = vertexShader;
}

void Entity::bindGeometryShader(ID3D11GeometryShader * geometryShader)
{
	m_geometryShader = geometryShader;
}

void Entity::bindPixelShader(ID3D11PixelShader * pixelShader)
{
	m_pixelShader = pixelShader;
}

void Entity::setRotation(DirectX::XMFLOAT3 rotation, float angle)
{
	m_angle.x = rotation.x * XMConvertToRadians(angle);
	m_angle.y = rotation.y * XMConvertToRadians(angle);
	m_angle.z = rotation.z * XMConvertToRadians(angle);
	resetAngles();
	buildMatrix();

}

void Entity::setRotation(float x, float y, float z, float angle)
{
	XMFLOAT3 rot(x, y, z);
	setRotation(rot, angle);
}

void Entity::rotate(DirectX::XMFLOAT3 rotation, float angle)
{
	m_angle.x += rotation.x * XMConvertToRadians(angle);
	m_angle.y += rotation.y * XMConvertToRadians(angle);
	m_angle.z += rotation.z * XMConvertToRadians(angle);
	resetAngles();
	buildMatrix();
}

void Entity::rotate(float x, float y, float z, float angle)
{
	XMFLOAT3 rot(x, y, z);
	rotate(rot, angle);
}

DirectX::XMFLOAT3 Entity::getRotation() const
{
	return m_angle;
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

DirectX::XMFLOAT3 Entity::getPosition() const
{
	return m_pos;
}

void Entity::setScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	if (m_scale.x < 0)
		m_scale.x = 0;
	if (m_scale.y < 0)
		m_scale.y = 0;
	if (m_scale.z < 0)
		m_scale.z = 0;

	buildMatrix();
}

void Entity::setScale(float x, float y, float z)
{
	XMFLOAT3 scale(x, y, z);
	setScale(scale);
}

void Entity::setScale(float s)
{
	setScale(s, s, s);
}

void Entity::scale(DirectX::XMFLOAT3 scale)
{
	m_scale = multiply(m_scale, scale);
	if (m_scale.x < 0)
		m_scale.x = 0;
	if (m_scale.y < 0)
		m_scale.y = 0;
	if (m_scale.z < 0)
		m_scale.z = 0;

	buildMatrix();
}

void Entity::scale(float x, float y, float z)
{
	XMFLOAT3 scale(x, y, z);
	this->scale(scale);
}

void Entity::scale(float s)
{
	scale(s, s, s);
}

DirectX::XMFLOAT3 Entity::getScale() const
{
	return m_scale;
}

void Entity::makeSphere()
{
	std::vector<Mesh> m = m_model->getMeshes();
	for (size_t i = 0; i < m.size(); i++)
	{
		std::vector<VERTEX> v = m[i].getVertices();
		for (size_t k = 0; k < v.size(); k++)
		{
			VERTEX ver = v[i];
			DirectX::XMFLOAT3 f3Vert = XMFLOAT3(ver.x, ver.y, ver.z);
			DirectX::XMVECTOR Vvert = DirectX::XMLoadFloat3(&f3Vert); 
			DirectX::XMVECTOR normalizedVert = DirectX::XMVector3Normalize(Vvert); 
			/*v[i].x = XMVectorGetX(normalizedVert); 
			v[i].y = XMVectorGetY(normalizedVert);
			v[i].z = XMVectorGetZ(normalizedVert);*/

			v[i].x = 0;
			v[i].y = 0; 
			v[i].z = 0; 
		}

	}


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

void Entity::setSamplerState(ID3D11SamplerState * ss)
{
	m_samplerState = ss;
}

void Entity::draw(ID3D11DeviceContext *& deviceContext)
{
	setShaders(deviceContext);
	UINT32 vertexSize = sizeof(VERTEX);
	UINT offset = 0;

	size_t nrOfDrawCalls = m_vertexBufferVector.size();
	
	std::vector<Mesh> mesh = m_model->getMeshes();


	for (size_t i = 0; i < nrOfDrawCalls; i++)
	{
		if (!m_model->isTerrain())
		{
			Material m = mesh[i].getMaterial();
			m_cBuffer.ambient = m.getAmbientLevel();
			m_cBuffer.diffuse = m.getDiffuseLevel();
			m_cBuffer.specular = m.getSpecularLevel();
			m_cBuffer.emissive = m.getEmissiveColor();
			m_cBuffer.dissolve = m.getDissolveLevel();
			m_cBuffer.opticalDensity = m.getOpticalDensity();
			m_cBuffer.sharpness = m.getSharpness();

			ID3D11ShaderResourceView* ambientTexture = mesh[i].getMaterial().getAmbientTexture();
			ID3D11ShaderResourceView* diffuseTexture = mesh[i].getMaterial().getDiffuseTexture();
			ID3D11ShaderResourceView* specular = mesh[i].getMaterial().getSpecularColorTexture();
			ID3D11ShaderResourceView* normalMap = mesh[i].getMaterial().getBumpMap();
			deviceContext->PSSetShaderResources(0, 1, &ambientTexture);
			deviceContext->PSSetShaderResources(1, 1, &diffuseTexture);
			deviceContext->PSSetShaderResources(2, 1, &specular);
			deviceContext->PSSetShaderResources(3, 1, &normalMap);
		}
		else
		{
			ID3D11ShaderResourceView* texture = m_model->getTextureResourceView();
			deviceContext->PSSetShaderResources(0, 1, &texture);
		}
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBufferVector[i], &vertexSize, &offset);
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


		deviceContext->PSSetSamplers(0, 1, &m_samplerState);
		
		if (!m_model->isTerrain())
			deviceContext->Draw(static_cast<UINT>(mesh[i].getVertices().size()), 0);
		else
			deviceContext->Draw(static_cast<UINT>(m_model->getNrOfVertices()), 0);
	}
	

	//UINT32 vertexSize = sizeof(VERTEX);
	//UINT offset = 0;
	//deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &vertexSize, &offset);
	//ID3D11ShaderResourceView* texture = m_model->getTextureResourceView();
	//if (m_model->getTextureSetting() == 0)
	//{
	//	ID3D11SamplerState* sampleState = m_model->getSampleState();
	//	deviceContext->PSSetSamplers(0, 1, &sampleState);
	//}

	//deviceContext->PSSetShaderResources(0, 1, &texture);

	//D3D11_MAPPED_SUBRESOURCE dataPtr;
	//deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dataPtr);

	////	Copy memory from CPU to GPU
	//memcpy(dataPtr.pData, &m_cBuffer, sizeof(CONSTANT_BUFFER));

	//// Unmap constant buffer so that we can use it again in the GPU
	//deviceContext->Unmap(m_constantBuffer, 0);
	//// set resources to shaders
	//deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	//deviceContext->GSSetConstantBuffers(0, 1, &m_constantBuffer);
	//deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);

	//deviceContext->Draw(m_model->getNrOfVertices(), 0);
}

XMFLOAT3 Entity::add(XMFLOAT3 tar, XMFLOAT3 adder) const
{
	XMFLOAT3 result;
	result.x = tar.x + adder.x;
	result.y = tar.y + adder.y;
	result.z = tar.z + adder.z;
	return result;
}

DirectX::XMFLOAT3 Entity::multiply(DirectX::XMFLOAT3 tar, DirectX::XMFLOAT3 adder) const
{
	XMFLOAT3 result;
	result.x = tar.x * adder.x;
	result.y = tar.y * adder.y;
	result.z = tar.z * adder.z;
	return result;
}

void Entity::buildMatrix()
{
	XMMATRIX translate = XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);

	//XMVECTOR axis = XMLoadFloat3(&m_rot);
	//XMMATRIX rotation = XMMatrixRotationAxis(axis, m_currentAngle);

	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&m_angle));
	XMVECTOR quat = XMQuaternionRotationMatrix(rotation);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	/*
	XMMATRIX m_worldMatrix = XMMatrixAffineTransformation(
		XMLoadFloat3(&m_scale), XMVectorZero(), quat, XMLoadFloat3(&m_pos));
		*/
	m_worldMatrix = scale * rotation * translate;

	m_cBuffer.WorldMatrix = XMMatrixTranspose(m_worldMatrix);
	m_cBuffer.WVPMatrix = XMMatrixTranspose(m_worldMatrix * m_viewMatrix * m_projectionMatrix);
	
}

void Entity::init()
{
	m_vertexBuffer = nullptr;
	m_constantBuffer = nullptr;
	m_model = nullptr;
	m_vertexShader = nullptr;
	m_geometryShader = nullptr;
	m_pixelShader = nullptr;

	m_angle = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

void Entity::resetAngles()
{
	float maxAngle = XMConvertToRadians(360.0f);

	if (m_angle.x < maxAngle * -1)
		m_angle.x += maxAngle;
	if (m_angle.x > maxAngle)
		m_angle.x -= maxAngle;
	if (m_angle.y < maxAngle * -1)
		m_angle.y += maxAngle;
	if (m_angle.y > maxAngle)
		m_angle.y -= maxAngle;
	if (m_angle.z < maxAngle * -1)
		m_angle.z += maxAngle;
	if (m_angle.z > maxAngle)
		m_angle.z -= maxAngle;
}

void Entity::setShaders(ID3D11DeviceContext *& deviceContext) const
{
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->HSSetShader(nullptr, nullptr, 0);
	deviceContext->DSSetShader(nullptr, nullptr, 0);
	deviceContext->GSSetShader(m_geometryShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
}

int Entity::getNrOfVertices() const
{
	return m_model->getNrOfVertices();
}

Model* Entity::getModel() const
{
	return m_model; 
}

ID3D11Buffer * Entity::getVertexBuffer() const
{
	return m_vertexBuffer;
}

const DirectX::XMMATRIX & Entity::getWorldMatrix() const
{
	return m_worldMatrix;
}

