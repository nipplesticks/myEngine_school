#ifndef STRUCTS_HPP
#define STRUCTS_HPP
#include <DirectXMath.h>
struct CONSTANT_BUFFER
{
	DirectX::XMMATRIX WVPMatrix;
	DirectX::XMMATRIX WorldMatrix;
};

#endif