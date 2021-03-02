///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: renderer.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef UBROTENGINEDX11_EXPORTS
#define UBROTENGINE_DX11_API __declspec(dllexport)
#else
#define UBROTENGINE_DX11_API __declspec(dllimport)
#endif


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
//#include <DirectXCollision.h>
#include <cstdint>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "asset_manager.h"
#include "direct3d.h"
#include "shader_manager.h"
#include "vertex_types.h"
#include "view_matrix_handler.h"

#include "header/scene_manager.h"


/////////////
// GLOBALS //
/////////////
const bool VSYNC_ENABLED = false;
const bool FULL_SCREEN = false;
const float SCREEN_DEPTH = 100.0F;
const float SCREEN_NEAR = 1.0F;
//extern float SCREEN_DEPTH;
//extern float SCREEN_NEAR;

namespace graphics
{

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: Renderer
/// The renderer is used to initiate rendering of all entities in the scene and supports
/// the following entity types:
///		- Colored
///////////////////////////////////////////////////////////////////////////////////////////////////
class Renderer
{

public:
	Renderer() = default;
	Renderer(const Renderer &other) = delete;
	Renderer(Renderer&& other) noexcept = delete;
	auto operator=(const Renderer& other) -> Renderer = delete;
	auto operator=(Renderer&& other) -> Renderer& = delete;
	~Renderer() = default;

	auto Initialize(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT;

	void Shutdown();

	auto Refresh(const GraphicSettings& settings) -> HRESULT;

	//auto RegisterShader(HWND hwnd, int shader_type) -> bool;
	auto RegisterModel(const std::string& filename) -> size_t;
	auto RegisterModelProcedural(assets::Procedural num) -> size_t;
	auto RegisterTexture(const std::string& filename, uint8_t components) -> size_t;

	auto GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&;

	/**
	 * Prepares the backbuffer and subsequently calls \c RenderScene.
	 * @param scene
	 */
	auto Process(const Scene &scene) -> HRESULT;

private:
	/**
	 * Iterates over all tiles and all entities of the scence and then renders them accordingly.
	 * To inrease performance, only entities in the field of view are rendered (frustum culling).
	 * @param scene The scene to render
	 * @param camera The camera to use for rendering (holds necessary matrices)
	 */
	auto RenderScene(const Scene &scene) -> HRESULT;

	/**
	* Activates the vertex and index buffers for the input assembler of the GPU which enables
	* this model to be rendered by shaders. This function also sets the topology used to
	* render the model, which is a triangle list (\c IASetPrimitiveTopology).
	* @param deviceContext
	*/
	template <class T>
	static void RenderModel(ID3D11DeviceContext* device_context, const vertices::Model &model);

//private:
	std::unique_ptr<Direct3D> m_direct3d{ nullptr };
	std::unique_ptr<ShaderManager> m_shader_manager{ nullptr };
	std::unique_ptr<assets::AssetManager> m_asset_manager{ nullptr };
	std::unique_ptr<ViewMatrixHandler> m_view_matrix_handler{ nullptr };
};

} // namespace graphics