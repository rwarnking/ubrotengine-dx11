#include "../header/asset_loader.h"

//////////////
// INCLUDES //
//////////////
#include <fstream>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
///////////////////////
// MY CLASS INCLUDES //
///////////////////////
//#include "DDSTextureLoader.h"
#include "../header/model_factory.h"


namespace io
{

namespace dx = DirectX;

auto AssetLoader::LoadTexture(
	ID3D11Device* device, const std::string& filename
) -> ID3D11ShaderResourceView*
{
	UNREFERENCED_PARAMETER(device);
	UNREFERENCED_PARAMETER(filename);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	/*
	// Be careful to specify a format when creating the dds file!
	auto result = DirectX::CreateDDSTextureFromFile(
		d3device,
		L"D:/07_Projekte/UBrotEngineX/UBrotEngineX/data/assets/textures/Pebbles.dds",
		nullptr,
		srv.GetAddressOf()
	);

	if (FAILED(result)) {
		//throw std::runtime_error("");
	}
	*/
	return srv.Detach();
}


template <class T>
auto AssetLoader::LoadModel(
	ID3D11Device* d3device, const std::string& filename, gv::Model &model
) -> bool
{
	// Vertex array
	std::vector<T> vertices{};
	// Indices array
	std::vector<uint32_t> indices{};

	if (!LoadModelFromOBJ<T>(filename, model, vertices, indices)) {
		return false;
	}
	return InitializeBuffers(d3device, model, vertices, indices);
}


template <class T>
auto AssetLoader::LoadModelProcedural(
	ID3D11Device* d3device, gv::Model& model, assets::Procedural pModel
) -> bool
{
	// Vertices array
	std::vector<T> vertices{};
	// Indices array
	std::vector<uint32_t> indices{};

	switch (pModel) 
	{
		case assets::Procedural::Plane: 
			ModelFactory::GeneratePlane<T>(model, vertices, indices); 
			break;
		case assets::Procedural::Cube: 
			ModelFactory::GenerateCube<T>(model, vertices, indices); 
			break;
		default: 
			ModelFactory::GenerateTriangle<T>(model, vertices, indices);
			break;
	}
	return InitializeBuffers(d3device, model, vertices, indices);
}


template <class T>
auto AssetLoader::LoadModelFromOBJ(
	const std::string& filename,
	gv::Model &model,
	std::vector<T>& vertices,
	std::vector<uint32_t>& indices
) -> bool
{
	// Read in the number of vertices, tex coords, normals, and faces so that the data
	// can be initialized with the exact sizes needed.
	auto counts = ReadFileCounts(filename);
	auto face_count{ std::get<3>(counts) };
	if (std::get<0>(counts) == 0 || face_count == 0) {
		return false;
	}

	auto result = LoadData<T>(
		filename, counts, vertices, indices
	);
	if (!result) {
		return false;
	}

	model.vertexCount = face_count * 3;
	model.indexCount = face_count * 3;

	return true;
}


auto AssetLoader::ReadFileCounts(
	const std::string& filename
) -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>
{
	// Initialize the counts.
	uint32_t vertex_count{ 0 };
	uint32_t texture_count{ 0 };
	uint32_t normal_count{ 0 };
	uint32_t face_count{ 0 };

	// Open the file.
	std::ifstream fin(filename);

	// Check if it was successful in opening the file.
	if (fin.fail()) {
		return std::make_tuple(0, 0, 0, 0);
	}

	// Read from the file and continue to read until the end of the file is reached.
	char input{};
	fin.get(input);
	while (!fin.eof()) {
		// If the line starts with 'v' then count either the vertex,
		// the texture coordinates, or the normal vector.
		if (input == 'v') {
			fin.get(input);
			if (input == ' ') {
				vertex_count++;
			}
			if (input == 't') {
				texture_count++;
			}
			if (input == 'n') {
				normal_count++;
			}
		}
		// If the line starts with 'f' then increment the face count.
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {
				face_count++;
			}
		}
		// Otherwise read in the remainder of the line.
		while (input != '\n') {
			fin.get(input);
		}
		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return std::make_tuple(vertex_count, texture_count, normal_count, face_count);
}


template <class T>
auto AssetLoader::LoadData(
	const std::string& filename,
	const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>& counts,
	std::vector<T>& vertices,
	std::vector<uint32_t>& indices
) -> bool
{
	// Extract the different counts for the elements
	std::vector<gv::Vector3> positions(std::get<0>(counts));
	std::vector<gv::Vector2> texcoords(std::get<1>(counts));
	std::vector<gv::Vector3> normals(std::get<2>(counts));
	auto face_count = std::get<3>(counts);
	std::vector<gv::Face> faces(face_count);
	
	// Initialize the indexes.
	int vertexIndex{ 0 };
	int texcoordIndex{ 0 };
	int normalIndex{ 0 };
	int faceIndex{ 0 };

	// Open the file.
	std::ifstream fin(filename);

	// Check if it was successful in opening the file.
	if (fin.fail()) {
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Convert to left hand coordinate system
	constexpr bool INVERT = true;

	char input{};
	char input2{};
	fin.get(input);
	while (!fin.eof()) {
		if (input == 'v') {
			fin.get(input);

			// Read in the vertices.
			if (input == ' ') {
				fin >> positions[vertexIndex].x;
				fin >> positions[vertexIndex].y;
				fin >> positions[vertexIndex].z;
				// Invert the Z vertex to change to left hand system.
				if (INVERT) {
					positions[vertexIndex].z = positions[vertexIndex].z * -1.0F;
				}
				vertexIndex++;
			}
			// Read in the texture uv coordinates.
			if (input == 't') {
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				// Invert the V texture coordinates to left hand system.
				if (INVERT) {
					texcoords[texcoordIndex].y = 1.0F - texcoords[texcoordIndex].y;
				}
				texcoordIndex++;
			}
			// Read in the normals.
			if (input == 'n') {
				fin >> normals[normalIndex].x;
				fin >> normals[normalIndex].y;
				fin >> normals[normalIndex].z;
				// Invert the Z normal to change to left hand system.
				if (INVERT) {
					normals[normalIndex].z = normals[normalIndex].z * -1.0F;
				}
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f') {
			fin.get(input);
			if (input == ' ') {
				// Read the face data in backwards to convert it from right hand system
				// to a left hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2;
				fin >> faces[faceIndex].nIndex3 >> faces[faceIndex].vIndex2 >> input2;
				fin >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2;
				fin >> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1;
				fin >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n') {
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	// TODO(rwarnking) currently not in use
	dx::XMFLOAT3 min = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);
	dx::XMFLOAT3 max = dx::XMFLOAT3(0.0F, 0.0F, 0.0F);

	// Create the vertex/index array.
	vertices.resize(face_count * 3);
	indices.resize(face_count * 3);

	int vIndex{ 0 };
	int tIndex{ 0 };
	int nIndex{ 0 };
	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0, j = 0; i < faceIndex; i++, j += 3) {
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;
		
		float red = positions[vIndex].x;
		float green = positions[vIndex].y;
		float blue = positions[vIndex].z;
		vertices[j] = gv::Create(
			dx::XMFLOAT3(positions[vIndex].x, positions[vIndex].y, positions[vIndex].z),
			dx::XMFLOAT4(red, green, blue, 1.0F),
			dx::XMFLOAT2(texcoords[tIndex].x, texcoords[tIndex].y),
			dx::XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z),
			dx::XMFLOAT3(),
			dx::XMFLOAT3()
		);

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		vertices[j + 1] = gv::Create(
			dx::XMFLOAT3(positions[vIndex].x, positions[vIndex].y, positions[vIndex].z),
			dx::XMFLOAT4(red, green, blue, 1.0F),
			dx::XMFLOAT2(texcoords[tIndex].x, texcoords[tIndex].y),
			dx::XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z),
			dx::XMFLOAT3(),
			dx::XMFLOAT3()
		);

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		vertices[j + 2] = gv::Create(
			dx::XMFLOAT3(positions[vIndex].x, positions[vIndex].y, positions[vIndex].z),
			dx::XMFLOAT4(red, green, blue, 1.0F),
			dx::XMFLOAT2(texcoords[tIndex].x, texcoords[tIndex].y),
			dx::XMFLOAT3(normals[nIndex].x, normals[nIndex].y, normals[nIndex].z),
			dx::XMFLOAT3(),
			dx::XMFLOAT3()
		);

		indices[j] = j;
		indices[j + 1] = j + 1;
		indices[j + 2] = j + 2;

		for (int k = 0; k < 3; k++) {
			if (vertices[j + k].position.x < min.x) {
				min.x = vertices[j + k].position.x;
			}
			if (vertices[j + k].position.x > max.x) {
				max.x = vertices[j + k].position.x;
			}

			if (vertices[j + k].position.y < min.y) {
				min.y = vertices[j + k].position.y;
			}
			if (vertices[j + k].position.y > max.y) {
				max.y = vertices[j + k].position.y;
			}

			if (vertices[j + k].position.z < min.z) {
				min.z = vertices[j + k].position.z;
			}
			if (vertices[j + k].position.z > max.z) {
				max.z = vertices[j + k].position.z;
			}
		}
	}

	return true;
}


template <class T>
auto AssetLoader::InitializeBuffers(
	ID3D11Device* d3device,
	gv::Model& model,
	std::vector<T>& vertices,
	const std::vector<uint32_t>& indices
) -> bool
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result{ S_OK };

	// Initialize a static vertex buffer description
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(T) * model.vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Initialize the subresource structure and pass the vertex data
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer and store it in m_vertexBuffer
	result = d3device->CreateBuffer(
		&vertexBufferDesc,
		&vertexData,
		model.vertexBuffer.GetAddressOf()
	);
	if (FAILED(result)) {
		return false;
	}

	// Initialize a static index buffer description
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * model.indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Initialize the subresource structure and pass the index data
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer and store it in m_indexBuffer
	result = d3device->CreateBuffer(&indexBufferDesc, &indexData, model.indexBuffer.GetAddressOf());
	return !FAILED(result);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template bool
AssetLoader::LoadModel<gv::ColVertex>(
	ID3D11Device* d3device, const std::string& fn, gv::Model &model
);
/*
template bool
LoadModel<gv::SimVertex>(std::string fn, gv::Model &model, assets::Procedural pModel);
template bool
LoadModel<gv::TexVertex>(std::string fn, gv::Model &model, assets::Procedural pModel);
template bool
LoadModel<gv::LigVertex>(std::string fn, gv::Model &model, assets::Procedural pModel);
template bool
LoadModel<gv::NomVertex>(std::string fn, gv::Model &model, assets::Procedural pModel);
template bool
LoadModel<gv::TesVertex>(std::string fn, gv::Model &model, assets::Procedural pModel);
*/

template bool
AssetLoader::LoadModelProcedural<gv::ColVertex>(
	ID3D11Device* d3device, gv::Model& model, assets::Procedural pModel
);

} // namespace io
