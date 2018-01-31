#include "Rendering.hpp"
#include "Character.h"

App::App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_lpCmdLine = lpCmdLine;
	m_nCmdShow = nCmdShow;
	SetCursorPos(static_cast<int>(CLIENT_WIDTH) / 2, static_cast<int>(CLIENT_HEIGHT) / 2);
	cam = CAMERA{
		DirectX::XMFLOAT3(0.0f, 117.0f, 0.0f),	//pos
		DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), // look at dir
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), //look at pos
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)		//up
	};
	m_viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&cam.Position),
		DirectX::XMLoadFloat3(&cam.LookAtDir),
		DirectX::XMLoadFloat3(&cam.Up)
	);

	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), CLIENT_WIDTH / CLIENT_HEIGHT, 0.1f, 20000.0f);


	//<TEST>
	//REMOVE THIS LATER
	
	m_CrouchLock = false;
	m_flying = true;
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
		loadEnteties();
		ShowWindow(m_wndHandle, m_nCmdShow);
	}

	return 0;
}

int App::run()
{


	MSG msg = { 0 };
	using namespace std::chrono;
	auto time = steady_clock::now();
	while (WM_QUIT != msg.message && !GetAsyncKeyState(VK_ESCAPE))
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			auto currentTime = steady_clock::now();
			auto dt = duration<float>(currentTime - time).count();
			time = steady_clock::now();

			Update(dt);
			Render();
			
			m_SwapChain->Present(0, 0);
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

void App::Update(float dt)
{
	if (GetAsyncKeyState(VK_LEFT))
		m_Statue.move(-5 * dt, 0, 0);
	if (GetAsyncKeyState(VK_RIGHT))
		m_Statue.move(5 * dt, 0, 0);
	if (GetAsyncKeyState(VK_UP))
		m_Statue.move(0, 0, 5*dt);
	if (GetAsyncKeyState(VK_DOWN))
		m_Statue.move(0, 0, -5 * dt);
	if (GetAsyncKeyState(VK_DELETE))
		m_Statue.move(0, -5 * dt, 0);
	if (GetAsyncKeyState(VK_END))
		m_Statue.move(0, 5 * dt, 0);
	//system("cls");
	//std::cout << m_Statue.getPosition().x << ":" << m_Statue.getPosition().y << ":" << m_Statue.getPosition().z;


	int defaultX = static_cast<int>(CLIENT_WIDTH) / 2;
	int defaultY = static_cast<int>(CLIENT_HEIGHT) / 2;	
	float speed = 10.0f;
	if (GetAsyncKeyState(VK_LSHIFT)) speed = 120.0f;

	if (m_CrouchLock)
		speed *= 0.4f;
	float sensitivity = 5.0f;
	if (GetAsyncKeyState(int('A')))
	{
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&cam.LookAtDir);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, v3);
		cam.Position.x -= speed * dir.x * dt;
		cam.Position.y -= speed * dir.y * dt;
		cam.Position.z -= speed * dir.z * dt;
	}
	if (GetAsyncKeyState(int('D')))
	{
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&cam.LookAtDir);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, v3);
		cam.Position.x += speed * dir.x * dt;
		cam.Position.y += speed * dir.y * dt;
		cam.Position.z += speed * dir.z * dt;
	}
	if (GetAsyncKeyState(int('W')))
	{
		DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&cam.LookAtDir);
		if (!m_flying)
		{
			DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&cam.Up);
			DirectX::XMVECTOR right = DirectX::XMVector3Cross(lookAt, up);
			DirectX::XMVECTOR front = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, right));
			DirectX::XMFLOAT3 dir;
			DirectX::XMStoreFloat3(&dir, front);
			cam.Position.x += speed * dir.x * dt;
			cam.Position.y += speed * dir.y * dt;
			cam.Position.z += speed * dir.z * dt;
		}
		else
		{
			lookAt = DirectX::XMVector3Normalize(lookAt);
			DirectX::XMFLOAT3 dir;
			DirectX::XMStoreFloat3(&dir, lookAt);

			cam.Position.x += speed * dir.x * dt;
			cam.Position.y += speed * dir.y * dt;
			cam.Position.z += speed * dir.z * dt;
		}
	}
	if (GetAsyncKeyState(int('S')))
	{
		DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&cam.LookAtDir);
		if (!m_flying)
		{
			DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&cam.Up);
			DirectX::XMVECTOR right = DirectX::XMVector3Cross(lookAt, up);
			DirectX::XMVECTOR front = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, right));
			DirectX::XMFLOAT3 dir;
			DirectX::XMStoreFloat3(&dir, front);
			cam.Position.x -= speed * dir.x * dt;
			cam.Position.y -= speed * dir.y * dt;
			cam.Position.z -= speed * dir.z * dt;
		}
		else
		{
			lookAt = DirectX::XMVector3Normalize(lookAt);
			DirectX::XMFLOAT3 dir;
			DirectX::XMStoreFloat3(&dir, lookAt);

			cam.Position.x -= speed * dir.x * dt;
			cam.Position.y -= speed * dir.y * dt;
			cam.Position.z -= speed * dir.z * dt;
		}
	}
	if (!m_flying)
	{
		if (GetAsyncKeyState(VK_LCONTROL))
		{
			if (!m_CrouchLock)
			{
				m_CrouchLock = true;
				cam.Position.x -= cam.Up.x * 2;
				cam.Position.y -= cam.Up.y * 2;
				cam.Position.z -= cam.Up.z * 2;
			}
		}
		else if (m_CrouchLock)
		{
			m_CrouchLock = false;
			cam.Position.x += cam.Up.x * 2;
			cam.Position.y += cam.Up.y * 2;
			cam.Position.z += cam.Up.z * 2;
		}
	}
	else
	{
		if (GetAsyncKeyState(VK_SPACE))
		{
			cam.Position.x += cam.Up.x * speed * dt;
			cam.Position.y += cam.Up.y * speed* dt;
			cam.Position.z += cam.Up.z * speed* dt;
		}
		else if (GetAsyncKeyState(VK_LCONTROL))
		{
			cam.Position.x -= cam.Up.x * speed* dt;
			cam.Position.y -= cam.Up.y * speed* dt;
			cam.Position.z -= cam.Up.z * speed* dt;
		}
	}
	POINT mousePos;
	GetCursorPos(&mousePos);
	if (mousePos.x != defaultX || mousePos.y != defaultY)
	{
		int deltaX = mousePos.x - defaultX;
		int deltaY = mousePos.y - defaultY;

		
		DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&cam.LookAtDir);
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 camXAxel;
		DirectX::XMStoreFloat3(&camXAxel, v3);
		DirectX::XMVECTOR newLookAt = DirectX::XMLoadFloat3(&cam.LookAtDir);
		float angleBeforeRot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(newLookAt), v2));
		newLookAt = DirectX::XMVector3Transform(newLookAt, DirectX::XMMatrixRotationNormal(v3, DirectX::XMConvertToRadians(sensitivity * deltaY * dt)));
		float angleAfterRot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(newLookAt), v2));
		
		float angle = angleAfterRot - angleBeforeRot;
		printf((std::to_string(angle) + "\n").c_str());
		if (abs(angle) < 1.0f)
		{
			DirectX::XMStoreFloat3(&cam.LookAtDir, newLookAt);
		}
		else
		{
			newLookAt = DirectX::XMLoadFloat3(&cam.LookAtDir);
		}
		newLookAt = DirectX::XMVector3Transform(newLookAt, DirectX::XMMatrixRotationNormal(v2, DirectX::XMConvertToRadians(sensitivity * deltaX * dt)));
		newLookAt = DirectX::XMVector3Normalize(newLookAt);
		DirectX::XMStoreFloat3(&cam.LookAtDir, newLookAt);
	}

	m_viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&cam.Position),
		DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&cam.LookAtDir), DirectX::XMLoadFloat3(&cam.Position)),
		DirectX::XMLoadFloat3(&cam.Up)
	);

	//TEST
	DirectX::XMMATRIX skyBoxViewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorZero(),
		DirectX::XMLoadFloat3(&cam.LookAtDir),
		DirectX::XMLoadFloat3(&cam.Up)
	);

	m_Cube.cameraMoved(skyBoxViewMatrix);
	m_Statue.cameraMoved(m_viewMatrix);
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_player.cameraMoved(m_viewMatrix); 
	SetCursorPos(defaultX, defaultY);
}

void App::Render()
{
	clrScrn();
	m_player.draw(m_DeviceContext); 
	m_Statue.draw(m_DeviceContext);
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
	m_Mh.loadModel("models/Cube.obj", "SkyBox", true, true, false, false);
	m_Mh.loadModel("models/nymph1.obj", "Statue", false, true);
	m_Mh.loadModel("HeightMap/planet_deluxe.data", "Hidden_Star", true, false, true);
	m_Mh.loadModel("models/dog.obj", "Dog", true, true);
}

void App::loadEnteties()
{
	m_Mh.getModel("SkyBox")->initTexture(L"models/SkyBox3.dds", m_Device);
	m_Cube.loadModel(m_Mh.getModel("SkyBox"));			//The cube model
	m_Statue.loadModel(m_Mh.getModel("Statue"));		//The statue model
	m_Mh.getModel("Hidden_Star")->initTexture(L"HeightMap/Hidden_star_02.dds", m_Device);	//This will be moved into model later
	m_Terrain2.loadModel(m_Mh.getModel("Hidden_Star"));	//The Terrain model
	m_player.loadModel(m_Mh.getModel("Dog"));		//The dog model


	//<TEST>
	m_Cube.bindVertexShader(m_VertexShader);
	m_Cube.bindGeometryShader(m_GeometryShader);
	m_Cube.bindPixelShader(m_PixelShaderJustBlue);
	m_Cube.setProjectionMatrix(m_projectionMatrix);
	m_Cube.cameraMoved(m_viewMatrix);
	m_Cube.loadBuffers(m_Device);
	m_Cube.setPosition(-5000.0f, -5000.0f, -5000.0f);
	m_Cube.setScale(10000.0f);

	m_Statue.bindVertexShader(m_VertexShader);
	m_Statue.bindGeometryShader(m_GeometryShader);
	m_Statue.bindPixelShader(m_PixelShader);
	m_Statue.setProjectionMatrix(m_projectionMatrix);
	m_Statue.cameraMoved(m_viewMatrix);
	m_Statue.loadBuffers(m_Device);
	//m_Statue.setPosition(494.247f, 230.87f, 546.431f);
	m_Statue.setPosition(0, 2, 0);
	m_Statue.setRotation(0.0f, 0.0f, 1.0f, 180.0f);
	m_Statue.rotate(0.0f, 1.0f, 0.0f, 180.0f);

	m_Terrain2.bindVertexShader(m_VertexShader);
	m_Terrain2.bindGeometryShader(m_GeometryShader);
	m_Terrain2.bindPixelShader(m_PixelShaderTexture);
	m_Terrain2.setProjectionMatrix(m_projectionMatrix);
	m_Terrain2.cameraMoved(m_viewMatrix);
	m_Terrain2.loadBuffers(m_Device);
	m_Terrain2.setScale(5, 25, 5);
	//m_Terrain2.setPosition(0.0f, 0.0f, 0.0f);

	m_player.bindVertexShader(m_VertexShader);
	m_player.bindGeometryShader(m_GeometryShader);
	m_player.bindPixelShader(m_PixelShader);
	m_player.setProjectionMatrix(m_projectionMatrix);
	m_player.cameraMoved(m_viewMatrix);
	m_player.loadBuffers(m_Device);
	m_player.setRotation(0.0f, 0.0f, 0.0f, 180.0f);
	m_player.rotate(0, 1, 0, -90);
	m_player.setScale(0.05f, 0.05f, 0.05f);
	m_player.setPosition(-5, 2, 0);

	//</TEST>

}

