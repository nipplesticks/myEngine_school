#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "Model.hpp"

class Entity
{
public:
	Entity();
	~Entity();

	void loadModel(Model* m);
	void loadBuffers(ID3D11Device*& device);

	void setRotation(DirectX::XMFLOAT3 axis, float angle);
	void setRotation(float x, float y, float z, float angle);
	void rotate(DirectX::XMFLOAT3 axis, float angle);
	void rotate(float x, float y, float z, float angle);

	void setPosition(DirectX::XMFLOAT3 position);
	void setPosition(float x, float y, float z);
	void move(DirectX::XMFLOAT3 position);
	void move(float x, float y, float z);

	void setScale(DirectX::XMFLOAT3 scale);
	void setScale(float x, float y, float z);
	void scale(DirectX::XMFLOAT3 scale);
	void scale(float x, float y, float z);

	void setProjectionMatrix(DirectX::XMMATRIX projection);
	void cameraMoved(DirectX::XMMATRIX view);

	int getNrOfVertices() const;
	ID3D11Buffer* getVertexBuffer() const;

	void draw(ID3D11DeviceContext*& deviceContext) const;
private:
	DirectX::XMFLOAT3 add(DirectX::XMFLOAT3 tar, DirectX::XMFLOAT3 adder) const;
	void buildMatrix();
	void init();
private:
	ID3D11Buffer*		m_vertexBuffer;
	ID3D11Buffer*		m_constantBuffer;
	Model*				m_model;

	DirectX::XMFLOAT3	m_rot;

	//<NOT A GOOD SOLUTION, NEED FIX>
	float				m_currentAngle;
	//</NOT A GOOD SOLUTION, NEED FIX>
	//Direct::XMFLOAT3	m_angle;

	DirectX::XMFLOAT3	m_pos;
	DirectX::XMFLOAT3	m_scale;

	DirectX::XMMATRIX	m_viewMatrix;
	DirectX::XMMATRIX	m_projectionMatrix;
	DirectX::XMMATRIX	m_worldMatrix;
	CONSTANT_BUFFER		m_cBuffer;
};

#endif