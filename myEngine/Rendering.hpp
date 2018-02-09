#ifndef RENDERING_HPP
#define RENDERING_HPP

#include "Terrain.hpp"
#include "Character.h"
#include "ModelHandler.hpp"
#include "Camera.hpp"
#include <istream>

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
	bool createCameraBuffer();
	bool InitRenderFunction();
	void InitGBuffer();

	void Update();
	void Render();
	void firstDrawPass();
	void draw();
	void secondDrawPass();
	void drawSky();
	

	bool initVertexShader();
	bool CreateVertexShader();
	bool initVertexShaderNoGS();
	bool initSkyboxVertexShader();
	bool initDeferredVertexShader();

	bool CreateGeometryShader();
	
	bool setSamplerState();

	bool CreatePixelShader();
	bool initDrawTexture();
	bool initPixelEverything();
	bool initSkyboxPixelShader();
	bool initDeferredPixelShader();
	bool initRasterizer(); 

	void clrScrn();

	void setMembersToNull();


	//Remove later
	void loadModels();
	void loadEntities();

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
	ID3D11RasterizerState* m_RasterizerState; 

	ID3D11Buffer*			m_VertexBuffer;
	ID3D11InputLayout*		m_VertexLayout;
	ID3D11InputLayout*		m_DeferredVertexLayout;
	ID3D11VertexShader*		m_VertexShader;
	ID3D11VertexShader*		m_VertexShaderNoGS;
	ID3D11VertexShader*		m_VertexShaderSkybox;

	ID3D11SamplerState*		m_samplerState;


	ID3D11GeometryShader*	m_GeometryShader;

	ID3D11PixelShader*		m_PixelShader;
	ID3D11PixelShader*		m_PixelShaderTexture;
	ID3D11PixelShader*		m_PixelShaderEverything;
	ID3D11PixelShader*		m_PixelShaderSkybox;

	ID3D11VertexShader*		m_DeferredVertexShader;
	ID3D11PixelShader*		m_DeferredPixelShader;

	ID3D11Buffer*			m_ConstantBuffer;
	ID3D11Buffer*			m_CameraBuffer; 


	//DEPTH BUFFER
	ID3D11DepthStencilView* m_Dsv; //View
	ID3D11Texture2D*		m_Dsb; //Buffer

	TEXTURE_RENDER_TARGET m_Gbuffer[GBUFFER_SIZE];


	//Test
	ModelHandler m_Mh;
	Terrain m_Terrain2;
	Character m_Test;
	Entity m_Skybox;
	Cam m_Camera;

	DirectX::XMMATRIX		m_viewMatrix;
	DirectX::XMMATRIX		m_projectionMatrix;
};



#endif