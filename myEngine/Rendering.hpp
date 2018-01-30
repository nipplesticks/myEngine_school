#ifndef RENDERING_HPP
#define RENDERING_HPP

#include "Entity.hpp"
#include "Character.h"
#include <istream>

#define CLIENT_WIDTH	1920.0f
#define CLIENT_HEIGHT	1080.0f
#define FOV				45.0f

class App
{

public:
	App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
	~App();

	/*
		Returns an integer wich describes where in the code the error is.
		1: Failed to Init window
		2: Failed to create Direct 3D Context
		3: Failed to create depth buffer
		4: Failed to create shaders
		5: Failed to create constant buffers
		6: Failed to initiate render function
	*/
	int init();

	int run();
private:
	bool InitWindow();
	HRESULT CreateDirect3DContext();
	bool CreateDepthBuffer();
	void SetViewport();
	bool CreateShaders();
	bool CreateConstantBuffer();
	bool InitRenderFunction();

	void initTerrain(Model& terrain); 

	void Update(float dt);
	void Render();

	bool CreateVertexShader();
	bool CreateGeometryShader();
	bool CreatePixelShader();

	void clrScrn();

	void setMembersToNull();

	bool initDrawNormal();
	bool initJustBlue();
	bool initDrawTexture();
private:
	HINSTANCE	m_hInstance;
	HINSTANCE	m_hPrevInstance;
	LPWSTR		m_lpCmdLine;
	int			m_nCmdShow;

	HWND		m_wndHandle;

	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	ID3D11RenderTargetView* m_BackbufferRTV;

	ID3D11Buffer*			m_VertexBuffer;
	ID3D11InputLayout*		m_VertexLayout;
	ID3D11VertexShader*		m_VertexShader;
	ID3D11GeometryShader*	m_GeometryShader;
	ID3D11PixelShader*		m_PixelShader;
	ID3D11PixelShader*		m_PixelShaderDrawNormal;
	ID3D11PixelShader*		m_PixelShaderJustBlue;
	ID3D11PixelShader*		m_PixelShaderTexture;

	ID3D11Buffer*			m_ConstantBuffer;

	//DEPTH BUFFER
	ID3D11DepthStencilView* m_Dsv; //View
	ID3D11Texture2D*		m_Dsb; //Buffer

	//Test
	Entity m_Statue;
	Entity m_Terrain;
	Entity m_Terrain2;
	Entity m_Cube;
	Character m_player; 

	std::vector<Model*> models;

	CAMERA cam;
	DirectX::XMMATRIX		m_viewMatrix;
	DirectX::XMMATRIX		m_projectionMatrix;

	//REMOVE LATER
	bool m_CrouchLock;
	bool m_flying;

};



#endif