///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: model_factory.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/model_factory.h"


//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace io
{

namespace dx = DirectX;

template <class T>
void ModelFactory::GenerateTriangle(
	gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices
)
{
	constexpr uint32_t vertexCount = 3;
	constexpr uint32_t indexCount = 3;

	model.vertexCount = vertexCount;
	model.indexCount = indexCount;

	// Allocate temporary arrays for vertex and index data
	vertices.resize(vertexCount);
	indices.resize(indexCount);

	const float TRIANGLE_SIZE = 1.0F;
	const dx::XMFLOAT4 TRIANGLE_COLOR(0.0F, 1.0F, 0.0F, 1.0F);

	// Fill the vertex array (triangle)
	// Bottom left
	vertices[0] = gv::Create(
		dx::XMFLOAT3(-TRIANGLE_SIZE, -TRIANGLE_SIZE, 0.0F),
		TRIANGLE_COLOR,
		dx::XMFLOAT2(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3()
	);

	// Top middle
	vertices[1] = gv::Create(
		dx::XMFLOAT3(0.0F, TRIANGLE_SIZE, 0.0F),
		TRIANGLE_COLOR,
		dx::XMFLOAT2(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3()
	);

	// Bottom right
	vertices[2] = gv::Create(
		dx::XMFLOAT3(TRIANGLE_SIZE, -TRIANGLE_SIZE, 0.0F),
		TRIANGLE_COLOR,
		dx::XMFLOAT2(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3(),
		dx::XMFLOAT3()
	);

	// Fill the index array
	indices[0] = 0;  // Bottom left
	indices[1] = 1;  // Top middle
	indices[2] = 2;  // Bottom right
}


template <class T>
void ModelFactory::GeneratePlane(
	gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices
)
{
	constexpr uint32_t vertexCount = 4 * 2;
	constexpr uint32_t indexCount = 6;

	model.vertexCount = vertexCount;
	model.indexCount = indexCount;

	// Allocate temporary arrays for vertex and index data
	vertices.resize(vertexCount);
	indices.resize(indexCount);

	const float PLANE_SIZE = 0.5F;
	const dx::XMFLOAT4 PLANE_COLOR(0.0F, 1.0F, 0.0F, 1.0F);
	// Fill the vertex array (triangle)
	// Bottom left
	vertices[0] = gv::Create(
		dx::XMFLOAT3(-PLANE_SIZE, -PLANE_SIZE, 0.0F),
		PLANE_COLOR,
		dx::XMFLOAT2(), dx::XMFLOAT3(),
		dx::XMFLOAT3(), dx::XMFLOAT3()
	);

	// Top left
	vertices[1] = gv::Create(
		dx::XMFLOAT3(-PLANE_SIZE, PLANE_SIZE, 0.0F),
		PLANE_COLOR,
		dx::XMFLOAT2(), dx::XMFLOAT3(),
		dx::XMFLOAT3(), dx::XMFLOAT3()
	);

	// Bottom right
	vertices[2] = gv::Create(
		dx::XMFLOAT3(PLANE_SIZE, -PLANE_SIZE, 0.0F),
		PLANE_COLOR,
		dx::XMFLOAT2(), dx::XMFLOAT3(),
		dx::XMFLOAT3(), dx::XMFLOAT3()
	);

	// Top right
	vertices[3] = gv::Create(
		dx::XMFLOAT3(PLANE_SIZE, PLANE_SIZE, 0.0F),
		PLANE_COLOR,
		dx::XMFLOAT2(), dx::XMFLOAT3(),
		dx::XMFLOAT3(), dx::XMFLOAT3()
	);

	// Fill the index array
	uint32_t i = 0;
	indices[i++] = 0;  // Bottom left
	indices[i++] = 1;  // Top left
	indices[i++] = 2;  // Bottom right
	indices[i++] = 3;  // Bottom right
	indices[i++] = 2;  // Top left
	indices[i++] = 1;  // Top right
}

template <class T>
void ModelFactory::GenerateCube(
	gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices
)
{
	GenerateTriangle<T>(model, vertices, indices);
}

template <class T>
void ModelFactory::GenerateSphere(
	gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices
)
{
	GenerateTriangle<T>(model, vertices, indices);
}

template <class T>
void ModelFactory::GenerateThorus(
	gv::Model& model, std::vector<T>& vertices, std::vector<uint32_t>& indices
)
{
	GenerateTriangle<T>(model, vertices, indices);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

template void
ModelFactory::GenerateTriangle<gv::ColVertex>(
	gv::Model& model, std::vector<gv::ColVertex>& vertices, std::vector<uint32_t>& indices
);

template void
ModelFactory::GeneratePlane<gv::ColVertex>(
	gv::Model& model, std::vector<gv::ColVertex>& vertices, std::vector<uint32_t>& indices
);

template void
ModelFactory::GenerateCube<gv::ColVertex>(
	gv::Model& model, std::vector<gv::ColVertex>& vertices, std::vector<uint32_t>& indices
);

} // namespace io
