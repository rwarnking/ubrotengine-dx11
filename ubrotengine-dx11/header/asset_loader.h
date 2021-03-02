///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: asset_loader.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <cstdint>
#include <d3d11.h>
#include <string>
#include <tuple>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "vertex_types.h"


namespace assets {
	enum class Procedural : uint8_t
	{
		Plane = 0,
		Triangle,
		Cube,
		Sphere,
		NUMBER
	};
} // namespace assets

namespace io
{
namespace gv = graphics::vertices;
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: AssetLoader
///////////////////////////////////////////////////////////////////////////////////////////////////
class AssetLoader
{

public:
	static auto LoadTexture(
		ID3D11Device* device, const std::string& filename
	) -> ID3D11ShaderResourceView*;

	template <class T>
	auto LoadModel(
		ID3D11Device* device, const std::string& filename, gv::Model& model
	) -> bool;

	template <class T>
	auto LoadModelProcedural(
		ID3D11Device* d3device, gv::Model& model, assets::Procedural pModel
	) -> bool;

private:
	template <class T>
	auto LoadModelFromOBJ(
		const std::string& filename,
		gv::Model& model,
		std::vector<T>& vertices, 
		std::vector<uint32_t>& indices
	) -> bool;

	static auto ReadFileCounts(
		const std::string& filename
	) -> std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>;

	template <class T>
	auto LoadData(
		const std::string& filename,
		const std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>& counts,
		std::vector<T>& vertices, 
		std::vector<uint32_t>& indices
	) -> bool;

	template <class T>
	auto InitializeBuffers(
		ID3D11Device* d3device,
		gv::Model& model,
		std::vector<T>& vertices,
		const std::vector<uint32_t>& indices
	) -> bool;

};

} // namespace io
