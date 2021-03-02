///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: direct3d.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/direct3d.h"


//////////////
// INCLUDES //
//////////////
#include <array>
#include <fstream>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

Direct3D::Direct3D() :
	m_viewport{},
	m_swapChain{ nullptr },
	m_device{ nullptr },
	m_deviceContext{ nullptr },
	m_renderTargetView{ nullptr },
	m_depthStencilBuffer{ nullptr },
	m_depthStencilState{ nullptr },
	m_depthDisabledStencilState{ nullptr },
	m_depthStencilView{ nullptr },
	m_rasterState{ nullptr },
	m_rasterStateWireframe{ nullptr },
	m_rasterStateNoCulling{ nullptr },
	m_alphaEnableBlendingState{ nullptr },
	m_alphaDisableBlendingState{ nullptr },
	m_alphaToCoverageBlendingState{ nullptr },
	m_projectionMatrix{},
	m_worldMatrix{},
	m_orthoMatrix{}
{
}


auto Direct3D::Initialize(const HWND& hwnd, const GraphicSettings& settings) -> HRESULT
{
	HRESULT result{ S_OK };
	

	m_vsyncEnabled = settings.v_sync;

	///////////////////////////////////
	// Videocard/Display information //
	///////////////////////////////////
	// In the first steps, the refresh rate of the monitor is determined by getting and then
	// querying the adapter for the refresh rate numerator and denominator corresponding to
	// the desired window dimensions.

	// Create a factory which can be used to create other DXGI objects
	Microsoft::WRL::ComPtr<IDXGIFactory> factory{ nullptr };
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
	if (FAILED(result)) {
		return result;
	}

	// Construct an adapter for the graphics card currently in use
	Microsoft::WRL::ComPtr<IDXGIAdapter> adapter{ nullptr };
	result = factory->EnumAdapters(0, adapter.GetAddressOf());
	if (FAILED(result)) {
		return result;
	}

	// Determine the primary monitor
	Microsoft::WRL::ComPtr<IDXGIOutput> adapter_output{ nullptr };
	result = adapter->EnumOutputs(0, adapter_output.GetAddressOf());
	if (FAILED(result)) {
		return result;
	}


	// https://docs.microsoft.com/en-us/windows/win32/api/dxgi1_2/nf-dxgi1_2-idxgioutput1-getdisplaymodelist1
	// Enumerate all display modes which satisfy the given parameters and
	// store their number numModes
	unsigned int numModes = 0;
	result = adapter_output->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		nullptr
	);
	if (FAILED(result)) {
		return result;
	}

	// Construct an array where all display modes can be stored
	std::vector<DXGI_MODE_DESC> display_mode_list;
	display_mode_list.resize(numModes);
	m_resolutions.reserve(numModes);

	// Same function as above, this time we store the display modes in displayModeList
	result = adapter_output->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED,
		&numModes,
		display_mode_list.data()
	);
	if (FAILED(result)) {
		return result;
	}

	for (auto& mode : display_mode_list)
	{
		m_resolutions.emplace_back(uint16_t(mode.Width), uint16_t(mode.Height));
	}

	// For the first start we search for the closest matching mode, since we can not know
	// what exactly the logic has chosen as resolution
	DXGI_MODE_DESC tmp_display_mode_desc{};
	tmp_display_mode_desc.Width = settings.window_width;
	tmp_display_mode_desc.Height = settings.window_height;
	adapter_output->FindClosestMatchingMode(&tmp_display_mode_desc, &m_display_mode_desc, nullptr);
	

	// Query the graphics card description and store it in adapterDesc
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return result;
	}
	
	// in binary it needs to be 1024
	// https://www.gamedev.net/forums/topic/681964-how-do-i-detect-the-available-video-memory/
	constexpr unsigned int B_PER_KB = 1024;
	constexpr unsigned int KB_PER_MB = 1024;
	// Store the memory size of the graphics card
	m_videoCardMemory = adapterDesc.DedicatedVideoMemory / B_PER_KB / KB_PER_MB;
	// Copy the graphics card description
	m_videoCardDescription = std::wstring(adapterDesc.Description);
	
	// Not in use since the syntaxhighlighting does not work if used
	// #include <span>
	// m_videoCardDescription = std::wstring(std::span{ adapterDesc.Description }.data());
	
	////////////////////////////////////
	// Setup swapchain and backbuffer //
	////////////////////////////////////
	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer_ptr{ nullptr };
	// Specify the DirectX version to be used (11)
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	auto swap_chain_desc = CreateSwapChainDesc(
		hwnd, settings.window_width, settings.window_height
	);
	

	UINT creation_flags = 0;
	// TODO(rwarnking) how to use this
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Create the swap chain, Direct3D device and Direct3D device context using the swap chain
	// description. The latter serve as interfaces for all DirectX functions.
	result = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		creation_flags, &featureLevel, 1,
		D3D11_SDK_VERSION, &swap_chain_desc, m_swapChain.GetAddressOf(),
		m_device.GetAddressOf(), nullptr, m_deviceContext.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	result = m_swapChain->SetFullscreenState(settings.fullscreen, nullptr);
	if (FAILED(result)) {
		return result;
	}

	// Set the swap chain's backbuffer
	result = m_swapChain->GetBuffer(
		0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer_ptr.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}

	// Create the necessary render target view
	result = m_device->CreateRenderTargetView(
		back_buffer_ptr.Get(),
		nullptr,
		m_renderTargetView.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}
	

	///////////////////////////
	// Depth buffer creation //
	///////////////////////////
	// Since the depth buffer is just a texture, create a texture for the depth buffer and
	// the depth stencil buffer.
	auto depth_buffer_desc = CreateTexture2dDesc(settings.window_width, settings.window_height);
	result = m_device->CreateTexture2D(
		&depth_buffer_desc,
		nullptr,
		m_depthStencilBuffer.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}


	//////////////////////////////////
	// Depth stencil state creation //
	//////////////////////////////////
	// Initialize the depth stencil description, where the z-test is enabled.
	auto depth_stencil_desc = CreateDepthStencilDesc(TRUE);
	result = m_device->CreateDepthStencilState(
		&depth_stencil_desc,
		m_depthStencilState.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Initialize the second depth stencil description, which is used for 2D rendering, where
	// the z-buffer is disabled (set DepthEnable to false).
	depth_stencil_desc = CreateDepthStencilDesc(FALSE);
	result = m_device->CreateDepthStencilState(
		&depth_stencil_desc,
		m_depthDisabledStencilState.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Give the device context the created depth stencil state
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

	
	/////////////////////////////////
	// Depth stencil view creation //
	/////////////////////////////////
	// In order to use the depth buffer as a depth stencil texture, we need a corresponding
	// view, which needs a description to be created.
	auto depth_stencil_view_desc = CreateDepthStencilViewDesc();
	result = m_device->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		&depth_stencil_view_desc,
		m_depthStencilView.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Set the render target to the depth stencil view
	m_deviceContext->OMSetRenderTargets(
		1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get()
	);

	
	//////////////////////////
	// Blend state creation //
	//////////////////////////
	// Initialie a blend state which enables alpha blending to render transparent objects
	auto blend_desc = CreateBlendDesc(TRUE, FALSE);
	result = m_device->CreateBlendState(
		&blend_desc,
		m_alphaEnableBlendingState.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Create a blend state description where alpha blending is disabled to easily switch
	// between the two different modes.
	blend_desc = CreateBlendDesc(FALSE, FALSE);
	result = m_device->CreateBlendState(
		&blend_desc,
		m_alphaDisableBlendingState.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Adjustment of the blend state description, where AlphaToCoverage is disabled, which
	// is important for fonts that use signed distance fields.
	blend_desc = CreateBlendDesc(TRUE, TRUE);
	result = m_device->CreateBlendState(
		&blend_desc,
		m_alphaToCoverageBlendingState.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	
	////////////////////////////
	// Raster state creation //
	///////////////////////////
	// Normal mode
	auto raster_desc = CreateRasterDesc(D3D11_CULL_BACK, D3D11_FILL_SOLID);
	result = m_device->CreateRasterizerState(&raster_desc, m_rasterState.GetAddressOf());
	if (FAILED(result)) {
		return result;
	}

	// Wireframe mode
	raster_desc = CreateRasterDesc(D3D11_CULL_BACK, D3D11_FILL_WIREFRAME);
	result = m_device->CreateRasterizerState(&raster_desc, m_rasterStateWireframe.GetAddressOf());
	if (FAILED(result)) {
		return result;
	}

	// Disabled back face culling
	raster_desc = CreateRasterDesc(D3D11_CULL_NONE, D3D11_FILL_SOLID);
	result = m_device->CreateRasterizerState(&raster_desc, m_rasterStateNoCulling.GetAddressOf());
	if (FAILED(result)) {
		return result;
	}

	m_deviceContext->RSSetState(m_rasterState.Get());

	
	///////////////////////
	// Viewport creation //
	///////////////////////
	// Set the viewport which is needed so that Direct3D can map the clip space coordinates
	// to render target space coordinates.
	D3D11_VIEWPORT viewport;
	viewport.Width = float(settings.window_width);
	viewport.Height = float(settings.window_height);
	viewport.MinDepth = 0.0F;
	viewport.MaxDepth = 1.0F;
	viewport.TopLeftX = 0.0F;
	viewport.TopLeftY = 0.0F;

	m_deviceContext->RSSetViewports(1, &viewport);


	/////////////////////
	// Matrix creation //
	/////////////////////
	// First time creating the render matrices
	CalculateMatrices(
		settings.screen_near, settings.screen_depth, settings.window_width, settings.window_height
	);

	return S_OK;
}


void Direct3D::Shutdown()
{
	m_swapChain->SetFullscreenState(FALSE, nullptr);
}


// https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/complete-code-sample-for-using-a-corewindow-with-directx
auto Direct3D::Refresh(const GraphicSettings& settings) -> HRESULT
{
	HRESULT result{ S_OK };


	m_vsyncEnabled = settings.v_sync;
	ReleaseBackBuffer();

	///////////////////////////////////
	// Swapchain + backbuffer update //
	///////////////////////////////////
	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer_ptr{ nullptr };

	// Here we expect the logic to use the GetSupportedResolutions function
	// such that it is not necessary to check if there is a valid display_mode,
	// since all of the resolutions in the list should be valid
	DXGI_MODE_DESC zero_refresh_rate = m_display_mode_desc;
	zero_refresh_rate.Width = settings.window_width;
	zero_refresh_rate.Height = settings.window_height;
	zero_refresh_rate.RefreshRate.Numerator = 0;
	zero_refresh_rate.RefreshRate.Denominator = 0;

	// Adjust to fullscreen if necessary
	// https://bell0bytes.eu/fullscreen/
	// TODO(rwarnking) explanation why this is not needed in fullscreen mode
	if (!settings.fullscreen) {
		result = m_swapChain->ResizeTarget(&zero_refresh_rate);
		if (FAILED(result)) {
			return result;
		}
	}

	result = m_swapChain->SetFullscreenState(settings.fullscreen, nullptr);
	if (FAILED(result)) {
		return result;
	}

	if (!settings.fullscreen) {
		result = m_swapChain->ResizeTarget(&zero_refresh_rate);
		if (FAILED(result)) {
			return result;
		}
	}

	result = m_swapChain->ResizeBuffers(
		0, settings.window_width, settings.window_height, 
		DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	);
	if (FAILED(result)) {
		return result;
	}


	// Set the swap chain's backbuffer
	result = m_swapChain->GetBuffer(
		0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(back_buffer_ptr.GetAddressOf())
	);
	if (FAILED(result)) {
		return result;
	}

	// Create the necessary render target view
	result = m_device->CreateRenderTargetView(
		back_buffer_ptr.Get(),
		nullptr,
		m_renderTargetView.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	
	///////////////////////////
	// Depth buffer creation //
	///////////////////////////
	// Since the depth buffer is just a texture, create a texture for the depth buffer and
	// the depth stencil buffer.
	auto depth_buffer_desc = CreateTexture2dDesc(settings.window_width, settings.window_height);
	result = m_device->CreateTexture2D(
		&depth_buffer_desc,
		nullptr,
		m_depthStencilBuffer.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}


	//////////////////////////////
	// Reset depth stencil view //
	//////////////////////////////
	auto depth_stencil_view_desc = CreateDepthStencilViewDesc();
	result = m_device->CreateDepthStencilView(
		m_depthStencilBuffer.Get(),
		&depth_stencil_view_desc,
		m_depthStencilView.GetAddressOf()
	);
	if (FAILED(result)) {
		return result;
	}

	// Set the render target to the depth stencil view
	m_deviceContext->OMSetRenderTargets(
		1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get()
	);

	
	//////////////////
	// Set viewport //
	//////////////////
	D3D11_VIEWPORT viewport;
	viewport.Width = float(settings.window_width);
	viewport.Height = float(settings.window_height);
	viewport.MinDepth = 0.0F;
	viewport.MaxDepth = 1.0F;
	viewport.TopLeftX = 0.0F;
	viewport.TopLeftY = 0.0F;

	m_deviceContext->RSSetViewports(1, &viewport);

	CalculateMatrices(
		settings.screen_near, settings.screen_depth, settings.window_width, settings.window_height
	);

	return S_OK;
}


void Direct3D::BeginScene(
	const float red, const float green,
	const float blue, const float alpha
)
{
	const std::array<float, 4> color = { red, green, blue, alpha };
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color.data());
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0F, 0);
}


void Direct3D::EndScene() const
{
	// Present the rendered image, either with or without vSnyc depending on the settings.
	if (m_vsyncEnabled) {
		m_swapChain->Present(1, 0);
	}
	else {
		m_swapChain->Present(0, 0);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// CREATORs
///////////////////////////////////////////////////////////////////////////////////////////////////
auto Direct3D::CreateSwapChainDesc(
	const HWND& hwnd,
	const uint16_t window_width, const uint16_t window_height
) const -> DXGI_SWAP_CHAIN_DESC
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	// Initialize the swap chain, which is needed to swap between the back and front buffer,
	// by creating a description.
	ZeroMemory(&swap_chain_desc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Set the swap chain up so that the back buffer has the same size as the window and
	// color values are stored using 32 bits.
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.BufferDesc.Width = window_width;
	swap_chain_desc.BufferDesc.Height = window_height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// If vSync is enabled, set the refresh rate numerator and denominator previously
	// read from the display mode.
	if (m_vsyncEnabled) {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = m_display_mode_desc.RefreshRate.Numerator;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = m_display_mode_desc.RefreshRate.Denominator;
	}
	// If vSnyc is not enabled, the refresh rate is set to 0
	// (this may result in tearing: https://de.wikipedia.org/wiki/Screen_Tearing)
	else {
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the window handle and disable multi-sampling
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = hwnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	// Create a windowed swapchain and change the setting afterwards
	// https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ns-dxgi-dxgi_swap_chain_desc
	swap_chain_desc.Windowed = TRUE;

	// Set the scan line ordering and scaling to unspecified and discard the back buffer
	// contents after presenting the rendered image.
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	// https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_chain_flag
	swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	return swap_chain_desc;
}


auto Direct3D::CreateTexture2dDesc(
	const unsigned int tex_width, const unsigned int tex_height
) -> D3D11_TEXTURE2D_DESC
{
	// Initialize the depth buffer description
	D3D11_TEXTURE2D_DESC texture2d_desc;
	ZeroMemory(&texture2d_desc, sizeof(D3D11_TEXTURE2D_DESC));

	texture2d_desc.Width = tex_width;
	texture2d_desc.Height = tex_height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// texture2d_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	return texture2d_desc;
}


auto Direct3D::CreateDepthStencilDesc(BOOL depth_enable) -> D3D11_DEPTH_STENCIL_DESC
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	ZeroMemory(&depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	constexpr UINT8 MAX_MASK = 0xFF;
	depth_stencil_desc.DepthEnable = depth_enable;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
	depth_stencil_desc.StencilEnable = TRUE;
	depth_stencil_desc.StencilReadMask = MAX_MASK;
	depth_stencil_desc.StencilWriteMask = MAX_MASK;

	// Fix settings for front-facing and back-facing pixels
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	return depth_stencil_desc;
}


auto Direct3D::CreateDepthStencilViewDesc() -> D3D11_DEPTH_STENCIL_VIEW_DESC
{
	// In order to use the depth buffer as a depth stencil texture, we need a corresponding
	// view, which needs a description to be created.
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	ZeroMemory(&depth_stencil_view_desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;
	return depth_stencil_view_desc;
}


auto Direct3D::CreateBlendDesc(BOOL blend_enable, BOOL alpha_to_coverage) -> D3D11_BLEND_DESC
{
	D3D11_BLEND_DESC blend_desc;
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	constexpr UINT8 WRITE_MASK = 0x0f;
	blend_desc.RenderTarget[0].BlendEnable = blend_enable;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = WRITE_MASK;

	blend_desc.AlphaToCoverageEnable = alpha_to_coverage;
	return blend_desc;
}


auto Direct3D::CreateRasterDesc(
	D3D11_CULL_MODE cull_mode, D3D11_FILL_MODE fill_mode
) -> D3D11_RASTERIZER_DESC
{
	D3D11_RASTERIZER_DESC raster_desc;
	ZeroMemory(&raster_desc, sizeof(D3D11_RASTERIZER_DESC));

	raster_desc.AntialiasedLineEnable = FALSE;
	raster_desc.CullMode = cull_mode;
	raster_desc.DepthBias = 0;
	raster_desc.DepthBiasClamp = 0.0F;
	raster_desc.DepthClipEnable = TRUE;
	raster_desc.FillMode = fill_mode;
	raster_desc.FrontCounterClockwise = FALSE;
	raster_desc.MultisampleEnable = FALSE;
	raster_desc.ScissorEnable = FALSE;
	raster_desc.SlopeScaledDepthBias = 0.0F;
	return raster_desc;
}


void Direct3D::CalculateMatrices(
	const float screen_near, const float screen_depth,
	const uint16_t window_width, const uint16_t window_height
)
{
	// Compute the projection martix which is used to map the 3D scence to the 2D viewport.
	// Store all matrices so that shaders can easily copy them on demand
	constexpr float DIV = 4.0F;
	float field_of_view = DirectX::XM_PI / DIV;
	float screen_aspect = float(window_width) / float(window_height);
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
		field_of_view, screen_aspect,
		screen_near, screen_depth
	);

	// Compute the world matrix which maps an objects vertices to the 3D scene. To start with,
	// it is initialized with the identity matrix since rotation, scale and translation of
	// the object are unknown at this point.
	m_worldMatrix = DirectX::XMMatrixIdentity();

	// Create an orthographic projection matrix used for 2D rendering.
	m_orthoMatrix = DirectX::XMMatrixOrthographicLH(
		float(window_width), float(window_height),
		screen_near, screen_depth
	);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// CLEANUP
///////////////////////////////////////////////////////////////////////////////////////////////////
void Direct3D::ReleaseBackBuffer()
{
	// Release all outstanding references to the swap chain's buffers.
	//m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	m_deviceContext->ClearState();

	// Release the render target view based on the back buffer:
	//m_renderTargetView->Release();
	m_renderTargetView.Reset();

	// The depth stencil will need to be resized, so release it (and view):
	//m_depthStencilBuffer->Release();
	m_depthStencilBuffer.Reset();
	//m_depthStencilView->Release();
	m_depthStencilView.Reset();

	// After releasing references to these resources, we need to call Flush() to 
	// ensure that Direct3D also releases any references it might still have to
	// the same resources - such as pipeline bindings.
	m_deviceContext->Flush();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// GETTER
///////////////////////////////////////////////////////////////////////////////////////////////////
auto Direct3D::GetDevice() const -> ID3D11Device*
{
	return m_device.Get();
}


auto Direct3D::GetDeviceContext() const -> ID3D11DeviceContext*
{
	return m_deviceContext.Get();
}


auto Direct3D::GetSwapChain() const -> IDXGISwapChain*
{
	return m_swapChain.Get();
}


auto Direct3D::GetProjectionMatrix() const -> const DirectX::XMMATRIX&
{
	return m_projectionMatrix;
}


auto Direct3D::GetWorldMatrix() const -> const DirectX::XMMATRIX
{
	return m_worldMatrix;
}


auto Direct3D::GetOrthoMatrix() const -> const DirectX::XMMATRIX&
{
	return m_orthoMatrix;
}


auto Direct3D::GetVideoCardName() const -> std::wstring
{
	return m_videoCardDescription;
}


auto Direct3D::GetVideoCardMemory() const -> size_t
{
	return m_videoCardMemory;
}


auto Direct3D::GetDepthStencilView() const -> ID3D11DepthStencilView*
{
	return m_depthStencilView.Get();
}


auto Direct3D::GetSupportedResolutions() const -> const std::vector<std::tuple<uint16_t, uint16_t>>&
{
	return m_resolutions;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// SETTER
///////////////////////////////////////////////////////////////////////////////////////////////////
void Direct3D::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView.Get());
}


void Direct3D::TurnZBufferOn()
{
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
}


void Direct3D::TurnZBufferOff()
{
	m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState.Get(), 1);
}


void Direct3D::TurnAlphaBlendingOn()
{
	const std::array<float, 4> blend_factor = { 0.0F, 0.0F, 0.0F, 0.0F };
	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState.Get(), blend_factor.data(), SAMPLE_MASK);
}


void Direct3D::TurnAlphaBlendingOff()
{
	const std::array<float, 4> blend_factor = { 0.0F, 0.0F, 0.0F, 0.0F };
	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState.Get(), blend_factor.data(), SAMPLE_MASK);
}


void Direct3D::TurnAlphaBlendingCoverageOn()
{
	const std::array<float, 4> blend_factor = { 0.0F, 0.0F, 0.0F, 0.0F };
	m_deviceContext->OMSetBlendState(m_alphaToCoverageBlendingState.Get(), blend_factor.data(), SAMPLE_MASK);
}


void Direct3D::TurnCullingOn()
{
	m_deviceContext->RSSetState(m_rasterState.Get());
}


void Direct3D::TurnCullingOff()
{
	m_deviceContext->RSSetState(m_rasterStateNoCulling.Get());
}


void Direct3D::TurnWireframeOn()
{
	m_deviceContext->RSSetState(m_rasterStateWireframe.Get());
}


void Direct3D::TurnWireframeOff()
{
	m_deviceContext->RSSetState(m_rasterState.Get());
}


void Direct3D::ResetViewport()
{
	m_deviceContext->RSSetViewports(1, &m_viewport);
}

} // namespace graphics
