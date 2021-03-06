﻿///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: vertextypes.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl\client.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

namespace vertices
{

namespace dx = DirectX;

struct Model
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer{ nullptr };
	unsigned int vertexCount{ 0 };
	unsigned int indexCount{ 0 };
};

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

struct Vertex
{
	float x, y, z;
};

struct Face
{
	int vIndex1, vIndex2, vIndex3;
	int tIndex1, tIndex2, tIndex3;
	int nIndex1, nIndex2, nIndex3;
};

/*
struct SilVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 normal;
};*/

struct SimVertex
{
	dx::XMFLOAT3 position;
};
/**
* Container for colored vertices with a 3D position an a RGBA-color component
*/
struct ColVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT4 color;
};

struct TexVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT2 uv;
};

struct LigVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT2 uv;
	dx::XMFLOAT3 normal;
};

struct NomVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT2 uv;
	dx::XMFLOAT3 normal;
	dx::XMFLOAT3 tangent;
	dx::XMFLOAT3 binormal;
};

struct TesVertex
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT2 uv;
	dx::XMFLOAT3 normal;
	dx::XMFLOAT3 tangent;
	dx::XMFLOAT3 binormal;
};

template <typename... Ts>
//using VertexTypes = ecs::MPL::TypeList<Ts...>;

//using VertexList = VertexTypes<SimVertex, ColVertex, TexVertex, LigVertex, NomVertex, TesVertex>;

// Get index of type
//static_assert(ecs::MPL::IndexOf<ColVertex, VertexList>() == 1, "");
// Get type at index
//static_assert(std::is_same<ecs::MPL::Nth<2, VertexList>, TexVertex>{}, "");

// Concat type lists
//using v1 = VertexTypes<ColVertex>;
//using v2 = VertexTypes<TexVertex>;
//using vTypes = ecs::MPL::Impl::Concat<v1, v2>;
//static_assert(std::is_same <vTypes, ecs::MPL::TypeList<ColVertex, TexVertex>>{}, "Concat failed");


//		 - variable argument list results in internal compiler error on x64 in release mode
//			+ template usage (SFINAE, enable_if)

/*
static void Create(
	SimVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	UNREFERENCED_PARAMETER(color);
	UNREFERENCED_PARAMETER(uv);
	UNREFERENCED_PARAMETER(normal);
	UNREFERENCED_PARAMETER(tangent);
	UNREFERENCED_PARAMETER(binormal);
}
*/
/*
static void Create(
	ColVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	vert.color = color;
	UNREFERENCED_PARAMETER(uv);
	UNREFERENCED_PARAMETER(normal);
	UNREFERENCED_PARAMETER(tangent);
	UNREFERENCED_PARAMETER(binormal);
}*/

static auto Create(
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
) -> ColVertex
{
	ColVertex vert{};
	vert.position = pos;
	vert.color = color;
	UNREFERENCED_PARAMETER(uv);
	UNREFERENCED_PARAMETER(normal);
	UNREFERENCED_PARAMETER(tangent);
	UNREFERENCED_PARAMETER(binormal);
	return vert;
}

/*
static void Create(
	TexVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	vert.uv = uv;
	UNREFERENCED_PARAMETER(color);
	UNREFERENCED_PARAMETER(normal);
	UNREFERENCED_PARAMETER(tangent);
	UNREFERENCED_PARAMETER(binormal);
}


static void Create(
	LigVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	vert.uv = uv;
	vert.normal = normal;
	UNREFERENCED_PARAMETER(color);
	UNREFERENCED_PARAMETER(tangent);
	UNREFERENCED_PARAMETER(binormal);
}


static void Create(
	NomVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	vert.uv = uv;
	vert.normal = normal;
	vert.tangent = tangent;
	vert.binormal = binormal;
	UNREFERENCED_PARAMETER(color);
}


static void Create(
	TesVertex &vert,
	dx::XMFLOAT3 pos,
	dx::XMFLOAT4 color,
	dx::XMFLOAT2 uv,
	dx::XMFLOAT3 normal,
	dx::XMFLOAT3 tangent,
	dx::XMFLOAT3 binormal
)
{
	vert.position = pos;
	vert.uv = uv;
	vert.normal = normal;
	vert.tangent = tangent;
	vert.binormal = binormal;
	UNREFERENCED_PARAMETER(color);
}
*/

/*
//typename std::enable_if_t<std::is_same<T, TexVertex>::value>* = nullptr
template
<
	class T,
	typename std::enable_if<std::is_same<T, LigVertex>::value, LigVertex>::type* = nullptr
>
T Create(DirectX::XMFLOAT3 pos, ...)
{
	va_list args;
	va_start(args, pos);
	auto vertex = T();
	vertex.position = pos;

	va_arg(args, DirectX::XMFLOAT4);
	vertex.uv = va_arg(args, DirectX::XMFLOAT2);
	vertex.normal = va_arg(args, DirectX::XMFLOAT3);
	va_end(args);

	return vertex;
}
*/
} // namespace vertices

} // namespace graphics
