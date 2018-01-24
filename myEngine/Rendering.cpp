#include "Rendering.hpp"

App::App(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_lpCmdLine = lpCmdLine;
	m_nCmdShow = nCmdShow;
	SetCursorPos(static_cast<int>(CLIENT_WIDITH) / 2, static_cast<int>(CLIENT_HEIGHT) / 2);
	cam = CAMERA{
		DirectX::XMFLOAT3(0.0f, 3.0f, -10),	//pos
		DirectX::XMFLOAT3(0.0f, 3.0f, 0.0f),	//look at
		DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f)		//up
	};
	m_viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&cam.Position),
		DirectX::XMLoadFloat3(&cam.LookAt),
		DirectX::XMLoadFloat3(&cam.Up)
	);

	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(FOV), CLIENT_WIDITH / CLIENT_HEIGHT, 0.1f, 20000.0f);


	//<TEST>
	//REMOVE THIS LATER
	Model* m;
	m = new Model;
	m->settings(false, true);
	m->initModel("models/nymph1.obj");
	models.push_back(m);

	m = new Model;
	m->settings(true, false);
	m->initModel("models/Ring_Of_Doom.obj");
	models.push_back(m);
	m_Statue.loadModel(models[0]);
	m_Terrain.loadModel(models[1]);

	m_CrouchLock = false;
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

	//<TEST>
	int nrOfModels = models.size();
	for (int i = 0; i < nrOfModels; i++)
	{
		delete models[i];
	}
	for (int i = 0; i < nrOfModels; i++)
	{
		models.pop_back();
	}
	//</TEST>

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

		//<TEST>
		m_Statue.setProjectionMatrix(m_projectionMatrix);
		m_Statue.cameraMoved(m_viewMatrix);
		m_Statue.loadBuffers(m_Device);
		m_Statue.setPosition(0.0f, 3.0f, 0.0f);
		m_Statue.setRotation(0.0f, 0.0f, 1.0f, 180.0f);

		m_Terrain.setProjectionMatrix(m_projectionMatrix);
		m_Terrain.cameraMoved(m_viewMatrix);
		m_Terrain.loadBuffers(m_Device);
		m_Terrain.setPosition(-40.0f, -2.0f, -40.0f);
		m_Terrain.setRotation(1.0f, 0.0f, 1.0f, -90.0f);
		m_Terrain.setScale(100.0f);
		//</TEST>

		if (!CreateConstantBuffer()) return 5;
		if (!InitRenderFunction()) return 6;

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

	RECT rc = { 0, 0, static_cast<long>(CLIENT_WIDITH), static_cast<long>(CLIENT_HEIGHT) };
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
	dsd.Width = static_cast<UINT>(CLIENT_WIDITH);
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
	vp.Width = CLIENT_WIDITH;
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
	
	m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	m_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	m_DeviceContext->GSSetShader(m_GeometryShader, nullptr, 0);
	m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	m_DeviceContext->IASetInputLayout(m_VertexLayout);
	m_DeviceContext->OMSetRenderTargets(1, &m_BackbufferRTV, m_Dsv);
	return true;
}

void App::Update(float dt)
{
	/*
	if (GetAsyncKeyState(int('D')))
		m_Male.rotate(0, 1, 0, -100 * dt);
	*/
	int defaultX = static_cast<int>(CLIENT_WIDITH) / 2;
	int defaultY = static_cast<int>(CLIENT_HEIGHT) / 2;	
	float speed = 10.0f;
	if (m_CrouchLock)
		speed *= 0.6;
	float sensitivity = 12.0f;

	if (GetAsyncKeyState(int('A')))
	{
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cam.LookAt), DirectX::XMLoadFloat3(&cam.Position));
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, v3);
		std::cout << dir.x << dir.y << dir.z << std::endl;
		cam.Position.x -= speed * dir.x * dt;
		cam.Position.y -= speed * dir.y * dt;
		cam.Position.z -= speed * dir.z * dt;
		cam.LookAt.x -= speed * dir.x * dt;
		cam.LookAt.y -= speed * dir.y * dt;
		cam.LookAt.z -= speed * dir.z * dt;
	}
	if (GetAsyncKeyState(int('D')))
	{
		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cam.LookAt), DirectX::XMLoadFloat3(&cam.Position));
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, v3);
		cam.Position.x += speed * dir.x * dt;
		cam.Position.y += speed * dir.y * dt;
		cam.Position.z += speed * dir.z * dt;
		cam.LookAt.x += speed * dir.x * dt;
		cam.LookAt.y += speed * dir.y * dt;
		cam.LookAt.z += speed * dir.z * dt;
	}
	if (GetAsyncKeyState(int('W')))
	{
		DirectX::XMVECTOR lookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cam.LookAt), DirectX::XMLoadFloat3(&cam.Position));
		DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR right = DirectX::XMVector3Cross(lookAt, up);
		DirectX::XMVECTOR front = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, right));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, front);
		std::cout << dir.x << dir.y << dir.z << std::endl;
		cam.Position.x += speed * dir.x * dt;
		cam.Position.y += speed * dir.y * dt;
		cam.Position.z += speed * dir.z * dt;
		cam.LookAt.x += speed * dir.x * dt;
		cam.LookAt.y += speed * dir.y * dt;
		cam.LookAt.z += speed * dir.z * dt;
	}
	if (GetAsyncKeyState(int('S')))
	{
		DirectX::XMVECTOR lookAt = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cam.LookAt), DirectX::XMLoadFloat3(&cam.Position));
		DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR right = DirectX::XMVector3Cross(lookAt, up);
		DirectX::XMVECTOR front = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(up, right));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, front);
		std::cout << dir.x << dir.y << dir.z << std::endl;
		cam.Position.x -= speed * dir.x * dt;
		cam.Position.y -= speed * dir.y * dt;
		cam.Position.z -= speed * dir.z * dt;
		cam.LookAt.x -= speed * dir.x * dt;
		cam.LookAt.y -= speed * dir.y * dt;
		cam.LookAt.z -= speed * dir.z * dt;
	}

	if (GetAsyncKeyState(VK_LCONTROL))
	{
		if (!m_CrouchLock)
		{
			m_CrouchLock = true;
			cam.Position.x -= cam.Up.x * 2;
			cam.Position.y -= cam.Up.y * 2;
			cam.Position.z -= cam.Up.z * 2;
			cam.LookAt.x -= cam.Up.x * 2;
			cam.LookAt.y -= cam.Up.y * 2;
			cam.LookAt.z -= cam.Up.z * 2;
		}
	}
	else if (m_CrouchLock)
	{
		m_CrouchLock = false;
		cam.Position.x += cam.Up.x * 2;
		cam.Position.y += cam.Up.y * 2;
		cam.Position.z += cam.Up.z * 2;
		cam.LookAt.x += cam.Up.x * 2;
		cam.LookAt.y += cam.Up.y * 2;
		cam.LookAt.z += cam.Up.z * 2;
	}

	POINT mousePos;
	GetCursorPos(&mousePos);
	if (mousePos.x != defaultX || mousePos.y != defaultY)
	{
		int deltaX = mousePos.x - defaultX;
		int deltaY = mousePos.y - defaultY;

		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&cam.LookAt), DirectX::XMLoadFloat3(&cam.Position));
		DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&cam.Up);
		DirectX::XMVECTOR v3 = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, v3);

		cam.LookAt.x += sensitivity * deltaX * dir.x * dt;
		cam.LookAt.y += sensitivity * deltaX * dir.y * dt;
		cam.LookAt.z += sensitivity * deltaX * dir.z * dt;

		cam.LookAt.x -= sensitivity * deltaY * cam.Up.x * dt;
		cam.LookAt.y -= sensitivity * deltaY * cam.Up.y * dt;
		cam.LookAt.z -= sensitivity * deltaY * cam.Up.z * dt;
	}

	m_viewMatrix = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&cam.Position),
		DirectX::XMLoadFloat3(&cam.LookAt),
		DirectX::XMLoadFloat3(&cam.Up)
	);
	m_Statue.cameraMoved(m_viewMatrix);
	m_Terrain.cameraMoved(m_viewMatrix);
	SetCursorPos(defaultX, defaultY);
}

void App::Render()
{
	clrScrn();
	m_Statue.draw(m_DeviceContext);
	m_Terrain.draw(m_DeviceContext);
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


	return true;
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

	m_ConstantBuffer = nullptr;

	m_Dsv = nullptr;
	m_Dsb = nullptr;
}

