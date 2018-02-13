#include "Rendering.hpp"
#include "Character.h"
#include "SphereIntersect.hpp"

App::App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_lpCmdLine = lpCmdLine;
	m_nCmdShow = nCmdShow;
	SetCursorPos(static_cast<int>(CLIENT_WIDTH) / 2, static_cast<int>(CLIENT_HEIGHT) / 2);
	
	m_Camera = Cam(
		DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f),		//pos
		DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),		// look at dir
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));		//up
	
	m_viewMatrix = m_Camera.getViewMatrix();

	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), CLIENT_WIDTH / CLIENT_HEIGHT, 0.1f, 100000.0f);




	m_Light.lightColor = DirectX::XMVectorSet(212.0f / 255, 235.0f / 255, 255.0f / 255, 1.0f);
	m_Light.lightPosition = DirectX::XMVectorSet(25.0f, 10.0f, 25.0f, 1);
	m_Light.strength = 1.0f;

	m_sphereTest = SphereIntersect(50, DirectX::XMFLOAT3(0, 0, 0));

	setMembersToNull();
}

App::~App()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_BackbufferRTV->Release();
	m_SwapChain->Release();
	m_Device->Release();
	m_DeviceContext->Release();
	m_Dsv->Release();
	m_Dsb->Release();

	DestroyWindow(m_wndHandle);
}

int App::init()
{
	if (!InitWindow()) return 1;

	if (m_wndHandle)
	{
		CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

		HRESULT hr = CreateDirect3DContext();
		if (FAILED(hr))	return 2;
		if (!CreateDepthBuffer()) return 3;
		SetViewport();
		if (!CreateShaders()) return 4;

		if (!CreateConstantBuffer()) return 5;
		if (!InitRenderFunction()) return 6;
		if (!setSamplerState()) return 7;
		if (!initRasterizer()) return 8; 

		loadModels();
		loadEntities();
		ShowWindow(m_wndHandle, m_nCmdShow);
	}

	return 0;
}

int App::run()
{

	ShowCursor(FALSE);
	MSG msg = { 0 };
	using namespace std::chrono;
	auto time = steady_clock::now();
	auto timer = steady_clock::now();
	int updates = 0;
	int fpsCounter = 0;
	float freq = 1000000000.0f / REFRESH_RATE;
	float unprocessed = 0;

	while (WM_QUIT != msg.message && !GetAsyncKeyState(VK_ESCAPE))
	{
		auto currentTime = steady_clock::now();
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			auto dt = duration_cast<nanoseconds>(currentTime - time).count();
			time = steady_clock::now();

			unprocessed += dt / freq;


			while (unprocessed > 1)
			{
				updates++;
				unprocessed -= 1;
				Update();
			}

			fpsCounter++;

			Render();
			
			m_SwapChain->Present(0, 0);
		}

		if (duration_cast<milliseconds>(steady_clock::now() - timer).count() > 1000)
		{
			//printf("\rFPS: %d TICK: %d", fpsCounter, updates);
			updates = 0;
			fpsCounter = 0;
			timer += milliseconds(1000);
		}

	}
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool App::InitWindow()
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = m_hInstance;
	wcex.lpszClassName = "myEngine_3D";
	if (!RegisterClassEx(&wcex))
	{
		m_wndHandle = false;
		return false;
	}

	RECT rc = { 0, 0, static_cast<long>(CLIENT_WIDTH), static_cast<long>(CLIENT_HEIGHT) };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	HWND handle = CreateWindow(
		"myEngine_3D",
		"myEngine 3D test",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		m_hInstance,
		nullptr);

	m_wndHandle = handle;
	return true;
}

HRESULT App::CreateDirect3DContext()
{
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //DEBUG

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = m_wndHandle;                           // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;   // windowed/full-screen mode

// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&m_SwapChain,
		&m_Device,
		NULL,
		&m_DeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		// use the back buffer address to create the render target
		m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_BackbufferRTV);
		pBackBuffer->Release();
	}
	return hr;
}

bool App::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width = static_cast<UINT>(CLIENT_WIDTH);
	dsd.Height = static_cast<UINT>(CLIENT_HEIGHT);
	dsd.MipLevels = 1;
	dsd.ArraySize = 1;
	dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.SampleDesc.Count = 1;
	dsd.SampleDesc.Quality = 0;
	dsd.Usage = D3D11_USAGE_DEFAULT;
	dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags = 0;

	HRESULT hr = 0;
	hr = m_Device->CreateTexture2D(&dsd, nullptr, &m_Dsb);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		//exit(-1);
		return false;
	}
	hr = m_Device->CreateDepthStencilView(m_Dsb, nullptr, &m_Dsv);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		//exit(-1);
		return false;
	}
	return true;
}

void App::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = CLIENT_WIDTH;
	vp.Height = CLIENT_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_DeviceContext->RSSetViewports(1, &vp);
}

bool App::CreateShaders()
{
	if (!CreateVertexShader()) return false;
	if (!CreateGeometryShader()) return false;
	if (!CreatePixelShader()) return false;

	return true;
}

bool App::CreateConstantBuffer()
{

	if (!createCameraBuffer())
		return false;
	if (!createLightBuffer())
		return false;

	return true;
}

bool App::InitRenderFunction()
{

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->IASetInputLayout(m_VertexLayout);
	InitGBuffer();
	return true;
}

void App::InitGBuffer()
{
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = (UINT)CLIENT_WIDTH;
	textureDesc.Height = (UINT)CLIENT_HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	for (size_t i = 0; i < GBUFFER_SIZE; i++)
	{
		m_Device->CreateTexture2D(&textureDesc, NULL, &m_Gbuffer[i].tx);
	}

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;


	for (size_t i = 0; i < GBUFFER_SIZE; i++)
	{
		m_Device->CreateRenderTargetView(m_Gbuffer[i].tx, &renderTargetViewDesc, &m_Gbuffer[i].rtv);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	for (size_t i = 0; i < GBUFFER_SIZE; i++)
	{
		m_Device->CreateShaderResourceView(m_Gbuffer[i].tx, &shaderResourceViewDesc, &m_Gbuffer[i].rsv);
	}
}

void App::Update()
{
	m_Camera.update();
	m_viewMatrix = m_Camera.getViewMatrix();
	m_Sphere.setScale(m_Light.strength * 10);
	DirectX::XMFLOAT3 p;
	DirectX::XMStoreFloat3(&p, m_Light.lightPosition);
	m_Sphere.setPosition(p);

	//m_Terrain2.rotate(0, 1, 0, 1.0f);
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_Test.cameraMoved(m_viewMatrix);
	m_Skybox.cameraMoved(m_Camera.getViewMatrixForBackground());
	m_Test.collisionHandling(m_Terrain2, 100);
	m_Cat.cameraMoved(m_viewMatrix); 
	m_Sphere.cameraMoved(m_viewMatrix);

	DirectX::XMFLOAT3 forward = m_Camera.getForward();
	DirectX::XMFLOAT3 right = m_Camera.getRight();
	
	float speed = 2;

	if (GetAsyncKeyState(VK_UP))
	{
		
		forward.x *= -1 * speed;
		forward.y *= -1 * speed;
		forward.z *= -1 * speed;
		m_Test.move(forward);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		forward.x *= speed;
		forward.y *= speed;
		forward.z *= speed;
		m_Test.move(forward);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		right.x *= speed;
		right.y *= speed;
		right.z *= speed;
		m_Test.move(right);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		right.x *= -1 * speed;
		right.y *= -1 * speed;
		right.z *= -1 * speed;
		m_Test.move(right);
	}

	if (GetAsyncKeyState(int('M')))
		m_Test.rotate(0, 1, 0, 0.4);
	if (GetAsyncKeyState(int('N')))
		m_Test.rotate(0, 1, 0, -0.4);
	if (GetAsyncKeyState(int('L')))
		m_Camera.setPosition(p);

	DirectX::XMFLOAT3 pos = m_Test.getPosition();



	pos.y += 20.0f;
	//m_Camera.setPosition(pos);
	m_CameraStruct.lookAt = XMLoadFloat3(&m_Camera.getLookAt());
	m_CameraStruct.pos = XMLoadFloat3(&m_Camera.getPosition());
	m_CameraStruct.viewMatrix = DirectX::XMMatrixTranspose(m_Camera.getViewMatrix());
	D3D11_MAPPED_SUBRESOURCE CamDataPtr;
	m_DeviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &CamDataPtr);
	memcpy(CamDataPtr.pData, &m_CameraStruct, sizeof(CAMERA_BUFFER));
	m_DeviceContext->Unmap(m_CameraBuffer, 0); 
	
	if (GetAsyncKeyState(int('U')))
	{
		m_Light.strength += 0.1f;
	}
	if (GetAsyncKeyState(int('J')))
	{
		m_Light.strength -= 0.1f;
	}

	D3D11_MAPPED_SUBRESOURCE LightDataPtr;
	m_DeviceContext->Map(m_LightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &LightDataPtr);
	memcpy(LightDataPtr.pData, &m_Light, sizeof(LIGHT_BUFFER));
	m_DeviceContext->Unmap(m_LightBuffer, 0);

	m_Cat.collisionHandling(m_Terrain2, 0); 
	m_sphereTest.setPosition(m_Cat.getPosition()); 
	bool intersect = false;
	if (GetAsyncKeyState(int('K')))
		intersect = m_sphereTest.checkIntersection(m_Camera.getPosition(), m_Camera.getLookAt()); 
	if (intersect)
	{
		DirectX::XMFLOAT3 moveOffset = DirectX::XMFLOAT3(5, 0, 5); 
		m_Cat.move(m_Camera.getForward().x * moveOffset.x,m_Camera.getForward().y * moveOffset.y, m_Camera.getForward().z * moveOffset.z); 
		std::cout << "HIT" << std::endl; 
	}
	


}

void App::Render()
{
	clearShaderResources();
	firstDrawPass();
	draw();
	clearTargets(); //Clear rendertargets
	secondDrawPass();

	drawSky();
}

void App::firstDrawPass()
{
	m_DeviceContext->IASetInputLayout(m_VertexLayout);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	float c1[4]{ 0.0f, 0.0f, 1.0f, 1.0f };
	float c2[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
	ID3D11RenderTargetView* renderTargets[GBUFFER_SIZE];

	for (int i = 0; i < GBUFFER_SIZE; i++)
	{
		renderTargets[i] = m_Gbuffer[i].rtv;
	}
	m_DeviceContext->OMSetRenderTargets(GBUFFER_SIZE, renderTargets, m_Dsv);

	m_DeviceContext->ClearRenderTargetView(m_Gbuffer[0].rtv, c1);

	for (int i = 1; i < GBUFFER_SIZE; i++)
	{
		m_DeviceContext->ClearRenderTargetView(m_Gbuffer[i].rtv, c2);
	}

	m_DeviceContext->ClearDepthStencilView(m_Dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void App::draw()
{
	sortRenderingQueue();

	for (Entity* e : m_renderingQueue)
	{
		e->draw(m_DeviceContext);
	}
	//std::cout << "----------------------\n";
	m_Sphere.draw(m_DeviceContext);
	m_Terrain2.draw(m_DeviceContext);
	m_Skybox.draw(m_DeviceContext);
	//std::cout << "----------------------\n";
	//system("cls");
}

void App::secondDrawPass()
{
	m_DeviceContext->IASetInputLayout(m_DeferredVertexLayout);
	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	float c[4]{ 0.0f, 0.0f, 1.0f, 1.0f };

	m_DeviceContext->VSSetShader(m_DeferredVertexShader, nullptr, 0);
	m_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	m_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	m_DeviceContext->GSSetShader(nullptr, nullptr, 0);
	m_DeviceContext->PSSetShader(m_DeferredPixelShader, nullptr, 0);

	m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRTV, NULL);
	m_DeviceContext->ClearRenderTargetView(m_BackbufferRTV, c);

	for (int i = 0; i < GBUFFER_SIZE; i++)
	{
		m_DeviceContext->PSSetShaderResources(i, 1, &m_Gbuffer[i].rsv);
	}

	m_DeviceContext->Draw(4, 0);
}

void App::drawSky()
{
	
}

bool App::CreateVertexShader()
{
	if (!initVertexShader())
		return false;
	if (!initVertexShaderNoGS())
		return false;
	if (!initSkyboxVertexShader())
		return false;
	if (!initDeferredVertexShader())
		return false;

	return true;
}

bool App::initVertexShader()
{
	ID3DBlob* pVS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"VertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&error			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		pVS->Release();
		OutputDebugString((char*)error->GetBufferPointer());
		return false;
	}
	if (FAILED(m_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_VertexShader)))
	{
		pVS->Release();
		return false;
	}
	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(m_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_VertexLayout)))
	{
		pVS->Release();
		return false;
	}

	pVS->Release();
	return true;
}

bool App::initVertexShaderNoGS()
{
	ID3DBlob* pVS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"VertexShaderNoGeometryShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&error			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		pVS->Release();
		OutputDebugString((char*)error->GetBufferPointer());
		return false;
	}
	if (FAILED(m_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_VertexShaderNoGS)))
	{
		pVS->Release();
		return false;
	}

	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(m_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_VertexLayout)))
	{
		pVS->Release();
		return false;
	}

	pVS->Release();
	return true;
}

bool App::initSkyboxPixelShader()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderSkyBox.hlsl", // filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"main",				// entry point
		"ps_5_0",			// shader model (target)
		0,					// shader compile options
		0,					// effect compile options
		&pPS,				// double pointer to ID3DBlob		
		&error				// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pPS->Release();
		return false;
	}
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderSkybox)))
	{
		pPS->Release();
		return false;
	}
	pPS->Release();

	return true;
}

bool App::initPixelShaderColor()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderColor.hlsl", // filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"main",				// entry point
		"ps_5_0",			// shader model (target)
		0,					// shader compile options
		0,					// effect compile options
		&pPS,				// double pointer to ID3DBlob		
		&error				// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pPS->Release();
		return false;
	}
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderColor)))
	{
		pPS->Release();
		return false;
	}
	pPS->Release();

	return true;
}

bool App::initDeferredPixelShader()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"DeferredPixelShader.hlsl", // filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"main",				// entry point
		"ps_5_0",			// shader model (target)
		0,					// shader compile options
		0,					// effect compile options
		&pPS,				// double pointer to ID3DBlob		
		&error				// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pPS->Release();
		return false;
	}
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_DeferredPixelShader)))
	{
		pPS->Release();
		return false;
	}

	pPS->Release();

	return true;
}

bool App::initRasterizer()
{
	D3D11_RASTERIZER_DESC  rasDesc; 
	rasDesc.FillMode = D3D11_FILL_SOLID; 
	//rasDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasDesc.CullMode = D3D11_CULL_NONE; 
	rasDesc.FrontCounterClockwise = false;
	rasDesc.DepthBias = 0; 
	rasDesc.SlopeScaledDepthBias = 0.0f; 
	rasDesc.DepthBiasClamp = 0.0f; 
	rasDesc.DepthClipEnable = true;
	rasDesc.ScissorEnable = false; 
	rasDesc.MultisampleEnable = false; 
	rasDesc.AntialiasedLineEnable = false; 
	HRESULT hr = m_Device->CreateRasterizerState(&rasDesc, &m_RasterizerState); 
	if (SUCCEEDED(hr))
	{
		m_DeviceContext->RSSetState(m_RasterizerState); 
	}
	else
	{
		std::cout << "Failed to initialize Rasterizer!" << std::endl; 
	}
	return SUCCEEDED(hr);
}

void App::sortRenderingQueue()
{
	int minIndex = 0;
	for (int i = 0; i < m_renderingQueue.size() - 1; i++)
	{
		for (int k = i + 1; k < m_renderingQueue.size(); k++)
		{
			float d1 = getDistance(m_renderingQueue[k]->getPosition(), m_Camera.getPosition());
			float d2 = getDistance(m_renderingQueue[minIndex]->getPosition(), m_Camera.getPosition());
			if (d1 < d2)
			{
				minIndex = k;
			}
		}

		Entity* temp = m_renderingQueue[i];
		m_renderingQueue[i] = m_renderingQueue[minIndex];
		m_renderingQueue[minIndex] = temp;
	}
}

bool App::createCameraBuffer()
{
	D3D11_BUFFER_DESC buffDesc; 
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(CAMERA_BUFFER);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = m_Device->CreateBuffer(&buffDesc, nullptr, &m_CameraBuffer); 

	if (FAILED(hr))
	{
		std::cout << "Failed to create camera buffer!" << std::endl; 
		return false;
	}
	m_DeviceContext->VSSetConstantBuffers(1, 1, &m_CameraBuffer);
	m_DeviceContext->GSSetConstantBuffers(1, 1, &m_CameraBuffer);
	m_DeviceContext->PSSetConstantBuffers(1, 1, &m_CameraBuffer);
	return true;
}

bool App::createLightBuffer()
{
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(LIGHT_BUFFER);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = m_Device->CreateBuffer(&buffDesc, nullptr, &m_LightBuffer);

	if (FAILED(hr))
	{
		std::cout << "Failed to create light buffer!" << std::endl;
		return false;
	}

	m_DeviceContext->VSSetConstantBuffers(2, 1, &m_LightBuffer);
	m_DeviceContext->PSSetConstantBuffers(2, 1, &m_LightBuffer);
	//USE CAMERA STRUCT INSTEAD? 
	return true;
}

bool App::initSkyboxVertexShader()
{
	ID3DBlob* pVS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"VertexShaderSkyBox.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&error			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		pVS->Release();
		OutputDebugString((char*)error->GetBufferPointer());
		return false;
	}
	if (FAILED(m_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_VertexShaderSkybox)))
	{
		pVS->Release();
		return false;
	}
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(m_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_VertexLayout)))
	{
		pVS->Release();
		return false;
	}
	pVS->Release();
	return true;
}

bool App::initDeferredVertexShader()
{
	ID3DBlob* pVS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"DeferredVertexShader.hlsl", // filename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",			// entry point
		"vs_5_0",		// shader model (target)
		0,				// shader compile options			// here DEBUGGING OPTIONS
		0,				// effect compile options
		&pVS,			// double pointer to ID3DBlob		
		&error			// pointer for Error Blob messages.
						// how to use the Error blob, see here
						// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		pVS->Release();
		OutputDebugString((char*)error->GetBufferPointer());
		return false;
	}
	if (FAILED(m_Device->CreateVertexShader(pVS->GetBufferPointer(), pVS->GetBufferSize(), nullptr, &m_DeferredVertexShader)))
	{
		pVS->Release();
		return false;
	}
	//create input layout (verified using vertex shader)
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(m_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_DeferredVertexLayout)))
	{
		pVS->Release();
		return false;
	}


	pVS->Release();

	return true;
}

bool App::CreateGeometryShader()
{
	ID3DBlob* pGS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"GeometryShader.hlsl", // filename
		nullptr,				// optional macros
		nullptr,				// optional include files
		"main",				// entry point
		"gs_5_0",				// shader model (target)
		0,						// shader compile options			// here DEBUGGING OPTIONS
		0,						// effect compile options
		&pGS,					// double pointer to ID3DBlob		
		&error					// pointer for Error Blob messages.
								// how to use the Error blob, see here
								// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);

	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pGS->Release();
		return false;
	}
	if (FAILED(m_Device->CreateGeometryShader(pGS->GetBufferPointer(), pGS->GetBufferSize(), nullptr, &m_GeometryShader)))
	{
		pGS->Release();
		return false;
	}
	pGS->Release();

	return true;
}

bool App::setSamplerState()
{
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = m_Device->CreateSamplerState(&samplerDesc, &m_samplerState);

	return SUCCEEDED(hr);
}

bool App::CreatePixelShader()
{
	if (!initDrawTexture())
		return false;
	if (!initPixelEverything())
		return false;
	if (!initSkyboxPixelShader())
		return false;
	if (!initDeferredPixelShader())
		return false;
	if (!initPixelShaderColor())
		return false;

	return true;
}

void App::clrScrn()
{
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_BackbufferRTV, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_Dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void App::clearTargets()
{
	ID3D11RenderTargetView* renderTargets[GBUFFER_SIZE] = { nullptr };

	m_DeviceContext->OMSetRenderTargets(GBUFFER_SIZE, renderTargets, NULL);
}

void App::clearShaderResources()
{
	ID3D11ShaderResourceView* renderTargets[GBUFFER_SIZE] = { nullptr };
	m_DeviceContext->PSSetShaderResources(0, GBUFFER_SIZE, renderTargets);
}

float App::getDistance(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
{
	DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);
	DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);
	DirectX::XMVECTOR distanceVector = DirectX::XMVectorSubtract(v1, v2);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(distanceVector);
	
	return abs(DirectX::XMVectorGetX(length));
}

void App::setMembersToNull()
{
	m_SwapChain = nullptr;
	m_Device = nullptr;
	m_DeviceContext = nullptr;
	m_BackbufferRTV = nullptr;

	m_VertexBuffer = nullptr;
	m_VertexLayout = nullptr;
	m_VertexShader = nullptr;
	m_GeometryShader = nullptr;
	m_PixelShader = nullptr;

	m_CameraBuffer = nullptr; 
	m_LightBuffer = nullptr;
	m_samplerState = nullptr;
	m_DeferredPixelShader = nullptr;
	m_DeferredVertexLayout = nullptr;
	m_DeferredVertexShader = nullptr;

	m_Dsv = nullptr;
	m_Dsb = nullptr;
}

bool App::initDrawTexture()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderDrawTexture.hlsl", // filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"main",				// entry point
		"ps_5_0",			// shader model (target)
		0,					// shader compile options
		0,					// effect compile options
		&pPS,				// double pointer to ID3DBlob		
		&error				// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pPS->Release();
		return false;
	}
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderTexture)))
	{
		pPS->Release();
		return false;
	}

	pPS->Release();

	return true;
}

bool App::initPixelEverything()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderWithEverything.hlsl", // filename
		nullptr,			// optional macros
		nullptr,			// optional include files
		"main",				// entry point
		"ps_5_0",			// shader model (target)
		0,					// shader compile options
		0,					// effect compile options
		&pPS,				// double pointer to ID3DBlob		
		&error				// pointer for Error Blob messages.
							// how to use the Error blob, see here
							// https://msdn.microsoft.com/en-us/library/windows/desktop/hh968107(v=vs.85).aspx
	);
	if (FAILED(hr))
	{
		OutputDebugString((char*)error->GetBufferPointer());
		pPS->Release();
		return false;
	}
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderEverything)))
	{
		pPS->Release();
		return false;
	}

	pPS->Release();

	return true;
}

void App::loadModels()
{
	m_Mh.loadModel("models/cyborg/", "cyborg.obj", m_Device, true, true);
	m_Mh.loadModel("models/SkyBox/", "skybox.obj", m_Device, true, true, false);
	m_Mh.loadModel("models/Cat/", "cat.obj", m_Device, true, false, true); 
	m_Mh.loadModel("models/SkyBox/", "Sphere.obj", m_Device, false, true);
}

void App::loadEntities()
{
	m_Test.loadModel(m_Mh.getModel("cyborg.obj"));
	m_Terrain2.initTerrainViaHeightMap("HeightMap/NewHeightMap.data", "Mountain", 15.0f, 100, 100, 0.5f);
	m_Terrain2.setTerrainTexture(L"HeightMap/sand.dds", m_Device);
	m_Skybox.loadModel(m_Mh.getModel("skybox.obj"));
	m_Cat.loadModel(m_Mh.getModel("cat.obj")); 
	m_Sphere.loadModel(m_Mh.getModel("Sphere.obj"));

	m_Skybox.setSamplerState(m_samplerState);
	m_Skybox.bindVertexShader(m_VertexShaderSkybox);
	m_Skybox.bindPixelShader(m_PixelShaderSkybox);
	m_Skybox.setProjectionMatrix(m_projectionMatrix);
	m_Skybox.cameraMoved(m_viewMatrix);
	m_Skybox.loadBuffers(m_Device);

	m_Sphere.setSamplerState(m_samplerState);
	m_Sphere.bindVertexShader(m_VertexShaderNoGS);
	m_Sphere.bindPixelShader(m_PixelShaderColor);
	m_Sphere.setProjectionMatrix(m_projectionMatrix);
	m_Sphere.cameraMoved(m_viewMatrix);
	m_Sphere.loadBuffers(m_Device);
	m_Sphere.cameraMoved(m_viewMatrix);
	m_Sphere.setScale(m_Light.strength);
	DirectX::XMFLOAT3 p;
	DirectX::XMStoreFloat3(&p, m_Light.lightPosition);
	m_Sphere.setPosition(p);


	m_Terrain2.setSamplerState(m_samplerState);
	m_Terrain2.bindVertexShader(m_VertexShader);
	m_Terrain2.bindGeometryShader(m_GeometryShader);
	m_Terrain2.bindPixelShader(m_PixelShaderTexture);
	m_Terrain2.setProjectionMatrix(m_projectionMatrix);
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_Terrain2.loadBuffers(m_Device);
	m_Terrain2.setScale(30, 50, 30);

	m_Test.setSamplerState(m_samplerState);
	m_Test.bindVertexShader(m_VertexShaderNoGS);
	m_Test.bindPixelShader(m_PixelShaderEverything);
	m_Test.setProjectionMatrix(m_projectionMatrix);
	m_Test.cameraMoved(m_viewMatrix);
	m_Test.loadBuffers(m_Device);
	m_Test.setPosition(0, 0, 0);
	m_Test.setScale(20);

	m_renderingQueue.push_back(&m_Test);

	m_Cat.setSamplerState(m_samplerState); 
	m_Cat.bindVertexShader(m_VertexShader);
	m_Cat.bindGeometryShader(m_GeometryShader);
	m_Cat.bindPixelShader(m_PixelShaderTexture);
	m_Cat.setProjectionMatrix(m_projectionMatrix); 
	m_Cat.cameraMoved(m_viewMatrix); 
	m_Cat.loadBuffers(m_Device); 
	m_Cat.setPosition(250, 0, 250);

	m_renderingQueue.push_back(&m_Cat);
}

