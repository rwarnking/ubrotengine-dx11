///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: ubrotengine_dx11.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/ubrotengine_dx11.h"


//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

UBROTENGINE_DX11_API auto createEngine() -> std::unique_ptr<Engine>
{
	return std::make_unique<Engine>(Engine());
}


auto Engine::RendererInit(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT
{
	// Create and initialize a renderer object used to render scenes
	m_renderer = std::make_unique<Renderer>();
	return m_renderer->Initialize(hwnd, settings);
}


void Engine::Shutdown()
{
	m_renderer->Shutdown();
	m_renderer = nullptr;
}


auto Engine::Refresh(const GraphicSettings& settings) -> HRESULT
{
	return m_renderer->Refresh(settings);
}

/*
auto Engine::RegisterShader(HWND hwnd, int shader_type) -> bool
{
	return m_renderer->RegisterShader(hwnd, shader_type);
}*/


auto Engine::RegisterModel(const std::string& filename) -> size_t
{
	return m_renderer->RegisterModel(filename);
}


auto Engine::RegisterModelProcedural(const uint8_t num) -> size_t
{
	return m_renderer->RegisterModelProcedural(assets::Procedural(num));
}


auto Engine::RegisterTexture(const std::string& filename, uint8_t components) -> size_t
{
	return m_renderer->RegisterTexture(filename, components);
}


auto Engine::GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&
{
	return m_renderer->GetSupportedResolutions();
}


auto Engine::RenderScene(const Scene& scene) -> HRESULT
{
	return m_renderer->Process(scene);
}

} // namespace graphics
