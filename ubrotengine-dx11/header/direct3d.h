///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: direct3d.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


/////////////
// LINKING //
/////////////
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


//////////////
// INCLUDES //
//////////////
#include <cstdint>
#include <d3d11.h>
#include <directxmath.h>
#include <string>
// TODO(rwarnking) why is this needed even though it is in the framework file?
// The app runner cant compile without it
#include <wrl\client.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "graphic_settings.h"


namespace graphics
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: Direct3D
/// The Direct3D class contains all components needed to use Direct3D.
///////////////////////////////////////////////////////////////////////////////////////////////////
class Direct3D
{
public:
	Direct3D();
	Direct3D(const Direct3D& other) = delete;
	Direct3D(Direct3D&& other) noexcept = delete;
	auto operator=(const Direct3D& other) -> Direct3D = delete;
	auto operator=(Direct3D&& other) -> Direct3D& = delete;
	~Direct3D() = default;

	/**
	 * Method that initializes all settings needed to use DirectX 11.
	 *
	 * Uses \p screenWidth and \p screenHeight to set the window dimensions and sets
	 * \p hwnd to be the handle to that window.
	 *
	 * @param hwnd handle to the window
	 * @param screenNear distance from camera to near plane
	 * @param screenDepth distance from camera to far plane
	 * @param screenWidth width of the window
	 * @param screenHeight height of the window
	 * @param vsync lock the monitor refresh rate if \c true, else render as fast as possible
	 * @param fullscreen use windowed mode if \c false, else fullscreen
	 * @return whether everything could be initialized successfully
	 */
	auto Initialize(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT;

	void Shutdown();

	/**
	 * Called at the beginning of a new frame. The depth values are erased from the depth buffer
	 * and the back buffer is colored according to the color specified by the parameters.
	 * @param red red componenent of the back buffer color (0,1)
	 * @param green green componenent of the back buffer color (0,1)
	 * @param blue blue componenent of the back buffer color (0,1)
	 * @param alpha alpha componenent (opacity) of the back buffer color (0,1)
	 */
	void BeginScene(
		float red, float green,
		float blue, float alpha
	);
	/**
	 * Tells the swap chain to present the rendered 3D scene. This method should be called at the
	 * end of the frame, after all objects have been drawn.
	 */
	void EndScene() const;

	auto Refresh(const GraphicSettings& settings) -> HRESULT;

	/**
	 * Returns \a m_device
	 * @return Direct3D device
	 */
	[[nodiscard]] auto GetDevice() const -> ID3D11Device*;

	/**
	 * Returns \a m_deviceContext
	 * @return Direct3D device context
	 */
	[[nodiscard]] auto GetDeviceContext() const -> ID3D11DeviceContext*;

	/**
	* Returns \a m_swapChain
	* @return Direct3D swap chain
	*/
	[[nodiscard]] auto GetSwapChain() const -> IDXGISwapChain*;

	[[nodiscard]] auto GetProjectionMatrix() const -> const DirectX::XMMATRIX&;
	[[nodiscard]] auto GetWorldMatrix() const -> const DirectX::XMMATRIX;
	[[nodiscard]] auto GetOrthoMatrix() const -> const DirectX::XMMATRIX&;


	/**
	 * Returns the video card memory in Megabyte.
	 * @return memory
	 */
	[[nodiscard]] auto GetVideoCardMemory() const -> size_t;
	/**
	 * Returns the video card description as wstring.
	 * @return card_name
	 */
	[[nodiscard]] auto GetVideoCardName() const -> std::wstring;
	[[nodiscard]] auto GetDepthStencilView() const -> ID3D11DepthStencilView*;

	auto GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&;

	/**
	 * This function sets the back buffer as the current render target, which may be necessary
	 * after using render-to-texture or similar methods.
	 */
	void SetBackBufferRenderTarget();

	/* Utility functions for settings */
	void TurnZBufferOn();
	void TurnZBufferOff();
	void TurnAlphaBlendingOn();
	void TurnAlphaBlendingCoverageOn();
	void TurnAlphaBlendingOff();
	void TurnCullingOn();
	void TurnCullingOff();
	void TurnWireframeOn();
	void TurnWireframeOff();

	void ResetViewport();

private:
	[[nodiscard]] auto CreateSwapChainDesc(
		const HWND & hwnd,
		uint16_t window_width, uint16_t window_height
	) const -> DXGI_SWAP_CHAIN_DESC;

	static auto CreateTexture2dDesc(
		unsigned int tex_width, unsigned int tex_height
	) -> D3D11_TEXTURE2D_DESC;

	static auto CreateDepthStencilDesc(BOOL depth_enable) -> D3D11_DEPTH_STENCIL_DESC;

	static auto CreateDepthStencilViewDesc() -> D3D11_DEPTH_STENCIL_VIEW_DESC;

	static auto CreateBlendDesc(BOOL blend_enable, BOOL alpha_to_coverage) -> D3D11_BLEND_DESC;

	static auto CreateRasterDesc(
		D3D11_CULL_MODE cull_mode, D3D11_FILL_MODE fill_mode
	) -> D3D11_RASTERIZER_DESC;
	
	void CalculateMatrices(
		float screen_near, float screen_depth,
		uint16_t window_width, uint16_t window_height
	);

	void ReleaseBackBuffer();

// private:
	const unsigned int SAMPLE_MASK = 0xffffffff;

	D3D11_VIEWPORT m_viewport;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthDisabledStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateWireframe;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterStateNoCulling;

	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaEnableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaDisableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaToCoverageBlendingState;

	std::vector<std::tuple<uint16_t, uint16_t>> m_resolutions{};

	DXGI_MODE_DESC m_display_mode_desc{};

	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;

	std::wstring m_videoCardDescription;
	size_t m_videoCardMemory{ 0 };

	bool m_vsyncEnabled{ true };
};

} // namespace graphics
