///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: ubrotengine_dx11.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef UBROTENGINEDX11_EXPORTS
#define UBROTENGINE_DX11_API __declspec(dllexport)
#else
#define UBROTENGINE_DX11_API __declspec(dllimport)
#endif


//////////////
// INCLUDES //
//////////////
#include <cstdint>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "header/scene_manager.h"

#include "../header/renderer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: Engine
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace graphics
{

class Engine
{

public:
	Engine() = default;
	Engine(const Engine& other) = delete;
	Engine(Engine&& other) = default; 
	auto operator=(const Engine& other) -> Engine = delete;
	auto operator=(Engine&& other) -> Engine& = default;
	UBROTENGINE_DX11_API ~Engine() = default;

	// Initialize a Renderer
	UBROTENGINE_DX11_API auto RendererInit(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT;

	UBROTENGINE_DX11_API void Shutdown();

	UBROTENGINE_DX11_API auto Refresh(const GraphicSettings& settings) -> HRESULT;

	//UBROTENGINE_DX11_API auto RegisterShader(
	//	HWND hwnd, int shader_type
	//) -> bool;

	UBROTENGINE_DX11_API auto RegisterModel(const std::string& filename) -> size_t;

	UBROTENGINE_DX11_API auto RegisterModelProcedural(uint8_t num) -> size_t;


	UBROTENGINE_DX11_API auto RegisterTexture(
		const std::string& filename, uint8_t components
	) -> size_t;

	UBROTENGINE_DX11_API auto GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&;

	// Initialize a Renderer
	UBROTENGINE_DX11_API auto RenderScene(
		const Scene& scene
	) -> HRESULT;

private:
	std::unique_ptr<Renderer> m_renderer;
};

extern UBROTENGINE_DX11_API auto createEngine() -> std::unique_ptr<Engine>;

} // namespace graphics
