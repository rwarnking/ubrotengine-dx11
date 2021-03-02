///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: asset_manager.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/asset_manager.h"


//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace assets
{

namespace gv = graphics::vertices;

auto AssetManager::GetModel(size_t model_index) -> const gv::Model&
{
#if _DEBUG
	assert(model_index < models.size());
#endif
	return models[model_index];
}


auto AssetManager::AddModel(ID3D11Device* device, const std::string& filename) -> std::size_t
{
	auto it = model_idx.find(filename);
	if (it != model_idx.end()) {
		return it->second;
	}

	// Load the model from the file
	auto model = graphics::vertices::Model();
	auto res = m_asset_loader->LoadModel<graphics::vertices::ColVertex>(device, filename, model);
	// TODO(rwarnking) what to do when the model can not be loaded
	assert(res);
	
	// Add the model to the storage system
	auto pos = models.size();
	models.push_back(std::move(model));
	model_idx.insert({ filename, pos });
	return pos;
}


auto AssetManager::AddModelProcedural(ID3D11Device* device, Procedural idx) -> std::size_t
{
	// TODO(rwarnking) test if this works
	std::string filename = std::to_string(uint8_t(idx));
	auto it = model_idx.find(filename);
	if (it != model_idx.end()) {
		return it->second;
	}

	auto model = graphics::vertices::Model();
	m_asset_loader->LoadModelProcedural<graphics::vertices::ColVertex>(
		device, model, idx
	);

	// Add the model to the storage system
	auto pos = models.size();
	models.push_back(std::move(model));
	model_idx.insert({ filename, pos });
	return pos;
}


auto AssetManager::AddTexture(
	ID3D11Device* device, const std::string& filename, uint8_t components
) -> size_t
{
	UNREFERENCED_PARAMETER(components);

	auto it = texture_idx.find(filename);
	if (it != texture_idx.end()) {
		return it->second;
	}

	auto *texture = m_asset_loader->LoadTexture(device, filename);
	auto pos = textures.size();
	textures.push_back(texture);
	texture_idx.insert({ filename, pos });
	return pos;
}


auto AssetManager::GetTexture(size_t textureIndex) -> ID3D11ShaderResourceView*
{
	return textures[textureIndex];
}

} // namespace assets
