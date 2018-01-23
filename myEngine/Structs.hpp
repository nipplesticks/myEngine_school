#ifndef STRUCTS_HPP
#define STRUCTS_HPP
#include <windows.h>
#include <chrono>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <DirectXMath.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include <vector>

struct CONSTANT_BUFFER
{
	DirectX::XMMATRIX WVPMatrix;
	DirectX::XMMATRIX WorldMatrix;
};

struct VERTEX
{
	float x, y, z;
	float u, v;
};

struct QUAD
{
	
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

struct CAMERA
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 LookAt;
	DirectX::XMFLOAT3 Up;
};

#endif