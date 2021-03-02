///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: asset_manager.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <map>
#include <string>
#include <vector>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "asset_loader.h"
#include "vertex_types.h"


namespace assets
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: AssetManager
///////////////////////////////////////////////////////////////////////////////////////////////////
class AssetManager
{

public:
	// Model stuff
	auto AddModel(ID3D11Device* device, const std::string& filename) -> size_t;
	auto AddModelProcedural(ID3D11Device* device, Procedural idx) -> size_t;

	auto GetModel(size_t model_index) -> const graphics::vertices::Model&;

	// Texture stuff
	auto AddTexture(
		ID3D11Device* device, const std::string& filename, uint8_t components
	) -> size_t;

	auto GetTexture(size_t textureIndex) -> ID3D11ShaderResourceView*;

private:
	std::vector<graphics::vertices::Model> models;
	std::vector<ID3D11ShaderResourceView*> textures;

	std::map<std::string, size_t> model_idx;
	std::map<std::string, size_t> texture_idx;

	std::unique_ptr<io::AssetLoader> m_asset_loader{ nullptr };

};

} // namespace assets 