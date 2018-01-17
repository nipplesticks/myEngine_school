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

struct CAMERA
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 LookAt;
	DirectX::XMFLOAT3 Up;
};

#endif