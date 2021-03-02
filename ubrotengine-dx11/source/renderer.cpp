///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: renderer.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/renderer.h"


//////////////
// INCLUDES //
//////////////
#include <fstream>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "../header/asset_loader.h"


namespace graphics
{

auto Renderer::Initialize(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT
{
	auto result{ S_OK };

	
	// Create and initialize the Direct3D object.
	m_direct3d = std::make_unique<Direct3D>();
	result = m_direct3d->Initialize(hwnd, settings);
	if (FAILED(result)) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return result;
	}


	// Create and initialize the shader manager.
	m_shader_manager = std::make_unique<ShaderManager>();
	result = m_shader_manager->Initialize(m_direct3d->GetDevice(), hwnd);
	if (FAILED(result)) {
		MessageBox(hwnd, L"Could not initialize Shader Manager", L"Error", MB_OK);
		return result;
	}

	m_view_matrix_handler = std::make_unique<ViewMatrixHandler>();

	m_asset_manager = std::make_unique<assets::AssetManager>();

	return result;
}


void Renderer::Shutdown()
{
	m_direct3d->Shutdown();
	m_shader_manager->Shutdown();
}


auto Renderer::Refresh(const GraphicSettings& settings) -> HRESULT
{
	return m_direct3d->Refresh(settings);
}

/*
auto Renderer::RegisterShader(HWND hwnd, int shader_type) -> bool
{
	// if shader_type is not loaded load
	return m_shader_manager->AddShader(m_direct3d->GetDevice(), hwnd, shader_type);
}
*/

auto Renderer::RegisterModel(const std::string& filename) -> size_t
{
	return m_asset_manager->AddModel(m_direct3d->GetDevice(), filename);
}


auto Renderer::RegisterModelProcedural(const assets::Procedural num) -> size_t
{
	if (num < assets::Procedural::NUMBER) {
		return m_asset_manager->AddModelProcedural(m_direct3d->GetDevice(), num);
	}
	return m_asset_manager->AddModelProcedural(m_direct3d->GetDevice(), assets::Procedural::Plane);
}


auto Renderer::RegisterTexture(const std::string& filename, uint8_t components) -> size_t
{
	return m_asset_manager->AddTexture(m_direct3d->GetDevice(), filename, components);
}


auto Renderer::GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&
{
	return m_direct3d->GetSupportedResolutions();
}


auto Renderer::Process(const Scene& scene) -> HRESULT
{
	auto result{ S_OK };

	const auto& user = scene.GetUser(0);
	const auto& pos = user.GetCamPos();
	const auto& look_at = user.GetCamLookDir();

	m_view_matrix_handler->RenderViewMatrix(
		pos[0], pos[1], pos[2], look_at[0], look_at[1], look_at[2]
	);

	// Clear the buffers
	m_direct3d->BeginScene(1.0F, 0.0F, 1.0F, 1.0F);

	result = RenderScene(scene);

	// Present the rendered scene to the screen.
	m_direct3d->EndScene();

	return result;
}

auto Renderer::RenderScene(const Scene& scene) -> HRESULT
{
	using DirectX::XMMatrixMultiply;
	using DirectX::XMMatrixScaling;
	using DirectX::XMMatrixRotationRollPitchYaw;
	using DirectX::XMMatrixTranslation;

	auto result{ S_OK };


	// Get the nessessary matrices from the camera and the direct3d object.
	const auto& viewMatrix = m_view_matrix_handler->GetViewMatrix();
	auto worldMatrix = m_direct3d->GetWorldMatrix();
	const auto& projectionMatrix = m_direct3d->GetProjectionMatrix();
	//auto orthoMatrix = m_direct3d->GetOrthoMatrix();

	m_direct3d->TurnZBufferOn();
	//m_direct3d->TurnCullingOn();
	//m_direct3d->TurnWireframeOn();

	// For all tiles in this scene
	for (const auto& tile : scene.GetTiles()) {
		// Get the tile coords to access the scene objects
		for (const auto& o : scene.GetObjects(tile.first)) {

			// Transform the Object.
			/*
			worldMatrix = XMMatrixMultiply(
				XMMatrixMultiply(
					XMMatrixScaling(cTransform.scale.x, cTransform.scale.y, cTransform.scale.z),
					XMMatrixRotationRollPitchYaw(cTransform.rotation.x, cTransform.rotation.y + rot, cTransform.rotation.z)),
				XMMatrixTranslation(cTransform.position.x, cTransform.position.y, cTransform.position.z)
			);*/
			const auto position = o.GetPosition();
			worldMatrix = XMMatrixMultiply(
				XMMatrixMultiply(
					XMMatrixScaling(1.0F, 1.0F, 1.0F),
					XMMatrixRotationRollPitchYaw(0.0F, 0.0F, 0.0F)),
				XMMatrixTranslation(position[0], position.y, position.z)
			);

			// TODO(rwarnking) ask the object for the shader
			constexpr size_t shader_prog_idx = 0;
			const auto& model = m_asset_manager->GetModel(o.GetModelIdx());
			RenderModel<graphics::vertices::ColVertex>(m_direct3d->GetDeviceContext(), model);
			result = m_shader_manager->GetShaderProgram(shader_prog_idx).Render(
				m_direct3d->GetDeviceContext(),
				worldMatrix, viewMatrix, projectionMatrix,
				model.indexCount
			);
			if (FAILED(result)) {
				return result;
			}
		}
	}
	m_direct3d->TurnZBufferOff();
	//m_direct3d->TurnCullingÓff();

	return result;
}


template <class T>
void Renderer::RenderModel(ID3D11DeviceContext* device_context, const vertices::Model &model)
{
	unsigned int stride = sizeof(T);
	unsigned int offset = 0;

	// Pass the vertex buffer to the input assembler
	device_context->IASetVertexBuffers(0, 1, model.vertexBuffer.GetAddressOf(), &stride, &offset);

	// Pass the inbdex buffer to the input assembler
	device_context->IASetIndexBuffer(model.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the rendering topology (triangle list)
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

} // namespace graphics
