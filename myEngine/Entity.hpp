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
	void bindVertexShader(ID3D11VertexShader* vertexShader);
	void bindGeometryShader(ID3D11GeometryShader* geometryShader);
	void bindPixelShader(ID3D11PixelShader* pixelShader);

	void setRotation(DirectX::XMFLOAT3 axis, float angle);
	void setRotation(float x, float y, float z, float angle);
	void rotate(DirectX::XMFLOAT3 axis, float angle);
	void rotate(float x, float y, float z, float angle);
	DirectX::XMFLOAT3 getRotation() const;

	void setPosition(DirectX::XMFLOAT3 position);
	void setPosition(float x, float y, float z);
	void move(DirectX::XMFLOAT3 position);
	void move(float x, float y, float z);
	DirectX::XMFLOAT3 getPosition() const;

	void setScale(DirectX::XMFLOAT3 scale);
	void setScale(float x, float y, float z);
	void setScale(float s);
	void scale(DirectX::XMFLOAT3 scale);
	void scale(float x, float y, float z);
	void scale(float s);
	DirectX::XMFLOAT3 getScale() const;

	void setProjectionMatrix(DirectX::XMMATRIX projection);
	void cameraMoved(DirectX::XMMATRIX view);

	void setSamplerState(ID3D11SamplerState* ss);

	int getNrOfVertices() const;
	Model* getModel() const; 
	ID3D11Buffer* getVertexBuffer() const;

	void draw(ID3D11DeviceContext*& deviceContext);

	const DirectX::XMMATRIX& getWorldMatrix() const;
private:
	DirectX::XMFLOAT3 add(DirectX::XMFLOAT3 tar, DirectX::XMFLOAT3 adder) const;
	DirectX::XMFLOAT3 multiply(DirectX::XMFLOAT3 tar, DirectX::XMFLOAT3 adder) const;

	void buildMatrix();
	void init();
	void resetAngles();
	void setShaders(ID3D11DeviceContext *& deviceContext) const;

private:

	std::vector<ID3D11Buffer*> m_vertexBufferVector;
	ID3D11SamplerState*			m_samplerState;


	ID3D11Buffer*			m_vertexBuffer;
	ID3D11Buffer*			m_constantBuffer;
	ID3D11Buffer*			m_constantBufferValues;
	ID3D11VertexShader*		m_vertexShader;
	ID3D11GeometryShader*	m_geometryShader;
	ID3D11PixelShader*		m_pixelShader;

	DirectX::XMFLOAT3		m_angle;

	DirectX::XMFLOAT3		m_pos;
	DirectX::XMFLOAT3		m_scale;

	DirectX::XMMATRIX		m_viewMatrix;
	DirectX::XMMATRIX		m_projectionMatrix;
	DirectX::XMMATRIX		m_worldMatrix;
	CONSTANT_BUFFER			m_cBuffer;
	CONSTANT_BUFFER2		m_cBufferValues;

protected:
	Model*					m_model;
};

#endif