#include "Rendering.hpp"
#include "Character.h"

App::App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_lpCmdLine = lpCmdLine;
	m_nCmdShow = nCmdShow;
	SetCursorPos(static_cast<int>(CLIENT_WIDTH) / 2, static_cast<int>(CLIENT_HEIGHT) / 2);
	
	m_Camera = Cam(
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),		//pos
		DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f),		// look at dir
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));		//up
	
	m_viewMatrix = m_Camera.getViewMatrix();

	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), CLIENT_WIDTH / CLIENT_HEIGHT, 0.1f, 100000.0f);


	//<TEST>
	//REMOVE THIS LATER
	
	m_CrouchLock = false;
	m_flying = false;
	//</TEST>

	setMembersToNull();
}

App::~App()
{
	//m_VertexBuffer->Release();
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
		HRESULT hr = CreateDirect3DContext();
		if (FAILED(hr))	return 2;
		if (!CreateDepthBuffer()) return 3;
		SetViewport();
		if (!CreateShaders()) return 4;

		if (!CreateConstantBuffer()) return 5;
		if (!InitRenderFunction()) return 6;


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
		NULL,
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
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(CONSTANT_BUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// check if the creation failed for any reason
	HRESULT hr = 0;
	hr = m_Device->CreateBuffer(&bufferDesc, nullptr, &m_ConstantBuffer);
	if (FAILED(hr))
	{
		// handle the error, could be fatal or a warning...
		//exit(-1);
		return false;
	}
	return true;
}

bool App::InitRenderFunction()
{

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->IASetInputLayout(m_VertexLayout);
	m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRTV, m_Dsv);
	return true;
}

void App::initTerrain(Model & terrain)
{
	
}

void App::Update()
{
	m_Camera.update();
	m_viewMatrix = m_Camera.getViewMatrix();
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_player.collisionHandling(m_Terrain2, 2048);
	m_Cube.cameraMoved(m_Camera.getViewMatrixForBackground());
	DirectX::XMFLOAT3 pos = m_player.getPosition();
	pos.y += 100;
	m_Camera.setPosition(pos);


	//printf("\rCam.X:%f, Cam.Y:%f, Cam.Z:%f ", pos.x, pos.y, pos.z);

	//std::cout << "Y:" << m_Camera.getPosition().y << std::endl; 
	//m_Cube.cameraMoved(m_viewMatrix);
	
	m_player.cameraMoved(m_viewMatrix); 
	m_Soviet.rotate(0, 1, 0, 0.25f);
	m_SovietVobble += 0.05f;
	float newPos = DirectX::XMScalarCos(m_SovietVobble) * 2;
	m_Soviet.move(0, newPos, 0);
	m_Soviet.cameraMoved(m_viewMatrix);
	DirectX::XMFLOAT3 forward = m_Camera.getForward();
	DirectX::XMFLOAT3 right = m_Camera.getRight();

	float speed = 10;

	if (GetAsyncKeyState(VK_UP))
	{
		
		forward.x *= -1 * speed;
		forward.y *= -1 * speed;
		forward.z *= -1 * speed;
		m_player.move(forward);
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		forward.x *= speed;
		forward.y *= speed;
		forward.z *= speed;
		m_player.move(forward);
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		right.x *= speed;
		right.y *= speed;
		right.z *= speed;
		m_player.move(right);
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		right.x *= -1 * speed;
		right.y *= -1 * speed;
		right.z *= -1 * speed;
		m_player.move(right);
	}
}

void App::Render()
{
	clrScrn();
	m_Soviet.draw(m_DeviceContext);
	m_player.draw(m_DeviceContext); 
	m_Terrain2.draw(m_DeviceContext); 
	m_Cube.draw(m_DeviceContext);

}

bool App::CreateVertexShader()
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	if (FAILED(m_Device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &m_VertexLayout)))
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

bool App::CreatePixelShader()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
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
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShader)))
	{
		pPS->Release();
		return false;
	}
	
	pPS->Release();

	bool f = initDrawNormal();
	f = initJustBlue();
	f = initDrawTexture();

	return f;
}

void App::clrScrn()
{
	float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_BackbufferRTV, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_Dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
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
	m_PixelShaderDrawNormal = nullptr;

	m_ConstantBuffer = nullptr;

	m_Dsv = nullptr;
	m_Dsb = nullptr;
}

bool App::initDrawNormal()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderDrawNormals.hlsl", // filename
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
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderDrawNormal)))
	{
		pPS->Release();
		return false;
	}

	pPS->Release();

	return true;
}

bool App::initJustBlue()
{
	//create pixel shader
	ID3DBlob* pPS = nullptr;
	ID3DBlob* error = nullptr;
	HRESULT hr = D3DCompileFromFile(
		L"PixelShaderJustBlue.hlsl", // filename
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
	if (FAILED(m_Device->CreatePixelShader(pPS->GetBufferPointer(), pPS->GetBufferSize(), nullptr, &m_PixelShaderJustBlue)))
	{
		pPS->Release();
		return false;
	}

	pPS->Release();

	return true;
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

void App::loadModels()
{
	m_Mh.loadModel("models/Cube.obj", "SkyBox", true, true, false);
	m_Mh.getModel("SkyBox")->initTexture(L"models/SkyBox3.dds", m_Device);

	m_Mh.loadModel("models/dog.obj", "Dog", true, true);

	m_Mh.loadModel("models/soviet.obj", "Soviet", true, true);
	m_Mh.getModel("Soviet")->initTexture(L"models/soviet.dds", m_Device);
}

void App::loadEntities()
{
	
	m_Cube.loadModel(m_Mh.getModel("SkyBox"));			//The cube model
	m_player.loadModel(m_Mh.getModel("Dog"));		//The dog model
	m_Soviet.loadModel(m_Mh.getModel("Soviet"));
	m_Terrain2.initTerrainViaHeightMap("HeightMap/m.data", "Mountain", 1.0f, 100, 100, 0.5f);
	m_Terrain2.setTerrainTexture(L"HeightMap/sand.dds", m_Device);

	//<TEST>
	m_Cube.bindVertexShader(m_VertexShader);
	m_Cube.bindGeometryShader(m_GeometryShader);
	m_Cube.bindPixelShader(m_PixelShaderJustBlue);
	m_Cube.setProjectionMatrix(m_projectionMatrix);
	m_Cube.cameraMoved(m_viewMatrix);
	m_Cube.loadBuffers(m_Device);
	m_Cube.setPosition(-35000.0f, -35000.0f, -35000.0f);
	m_Cube.setScale(70000.0f);

	m_Terrain2.bindVertexShader(m_VertexShader);
	m_Terrain2.bindGeometryShader(m_GeometryShader);
	m_Terrain2.bindPixelShader(m_PixelShaderTexture);
	m_Terrain2.setProjectionMatrix(m_projectionMatrix);
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_Terrain2.loadBuffers(m_Device);
	m_Terrain2.setScale(100, 5, 100);
	m_Terrain2.setPosition(0.0f, 0.0f, 0.0f);

	m_player.bindVertexShader(m_VertexShader);
	m_player.bindGeometryShader(m_GeometryShader);
	m_player.bindPixelShader(m_PixelShader);
	m_player.setProjectionMatrix(m_projectionMatrix);
	m_player.cameraMoved(m_viewMatrix);
	m_player.loadBuffers(m_Device);
	m_player.setRotation(0.0f, 0.0f, 0.0f, 180.0f);
	m_player.rotate(0, 1, 0, -90);
	m_player.setScale(1);
	m_player.setPosition(0, 0, 0);

	m_Soviet.bindVertexShader(m_VertexShader);
	m_Soviet.bindGeometryShader(m_GeometryShader);
	m_Soviet.bindPixelShader(m_PixelShaderTexture);
	m_Soviet.setProjectionMatrix(m_projectionMatrix);
	m_Soviet.cameraMoved(m_viewMatrix);
	m_Soviet.loadBuffers(m_Device);
	m_Soviet.setPosition(0, 0, 2000);
	//m_Soviet.setScale(6);
	//</TEST>

}

