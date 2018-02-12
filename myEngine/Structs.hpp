#ifndef STRUCTS_HPP
#define STRUCTS_HPP
#include <windows.h>
#include <chrono>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <DirectXMath.h>
#define CLIENT_WIDTH	1920.0f
#define CLIENT_HEIGHT	1080.0f
#define FOV				75.0f
#define REFRESH_RATE	60.0f
#define GBUFFER_SIZE	8
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <vector>

struct CAMERA_BUFFER
{
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR lookAt;
	DirectX::XMMATRIX viewMatrix;
};

struct CONSTANT_BUFFER
{
	DirectX::XMMATRIX WVPMatrix;
	DirectX::XMMATRIX WorldMatrix;
	DirectX::XMVECTOR ambient;
	DirectX::XMVECTOR diffuse;
	DirectX::XMVECTOR specular;
	DirectX::XMVECTOR emissive;
	float dissolve;
	float opticalDensity;
	float sharpness;
	float sphereRadius;
	DirectX::XMVECTOR spherePos; 
};

struct CONSTANT_BUFFER2
{
	DirectX::XMMATRIX WVPMatrix;
	DirectX::XMMATRIX WorldMatrix;
};

struct VERTEX
{
	float x, y, z;
	float u, v;
	float xN, yN, zN;
	float xBi, yBi, zBi;
	float xT, yT, zT;
};

struct V_IMPORT
{
	float x, y, z;
};
struct VN_IMPORT
{
	float x, y, z;
};
struct VT_IMPORT
{
	float u, v;
};
struct F_IMPORT
{
	int vIndex, vtIndex, vnIndex;
};
struct I_INPUT
{
	int arr[3];
};

struct TEXTURE_RENDER_TARGET
{
	ID3D11Texture2D* tx;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* rsv;
};

struct LIGHT_BUFFER
{
	DirectX::XMVECTOR lightPosition;
	DirectX::XMVECTOR lightColor;
	float strength;
};

#endif