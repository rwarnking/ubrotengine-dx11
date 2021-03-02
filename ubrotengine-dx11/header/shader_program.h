///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: shaderprogram.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include <wrl\client.h>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{
///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: ShaderProgram
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderProgram
{

public:
	enum class ShaderType : uint8_t
	{
		VertexShader = 0,
		GeometryShader,
		HullShader,
		DomainShader,
		FragmentShader,
		ComputeShader,
		NUMBER
	};

private:
	struct Shader {
		virtual auto Create(ID3D11Device* device, ID3D10Blob* shader_buffer) -> HRESULT = 0;
		virtual void Set(ID3D11DeviceContext* device_context) = 0;
		virtual ~Shader() = default;
	};

	struct PixelShader : Shader {
		inline static const char* const entry_point = "FragmentShader";
		inline static const char* const type = "ps_5_0";

		auto Create(ID3D11Device *device, ID3D10Blob *shader_buffer) -> HRESULT override {
			return device->CreatePixelShader(
				shader_buffer->GetBufferPointer(),
				shader_buffer->GetBufferSize(),
				nullptr,
				m_pixel_shader.GetAddressOf()
			);
		}

		void Set(ID3D11DeviceContext *device_context) override {
			device_context->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader{ nullptr };
	};

	struct VertexShader : Shader {
		inline static const char* const entry_point = "MVertexShader";
		inline static const char* const type = "vs_5_0";

		auto Create(ID3D11Device *device, ID3D10Blob *shader_buffer) -> HRESULT override {
			return device->CreateVertexShader(
				shader_buffer->GetBufferPointer(),
				shader_buffer->GetBufferSize(),
				nullptr,
				m_vertex_shader.GetAddressOf()
			);
		}

		void Set(ID3D11DeviceContext *device_context) override {
			device_context->VSSetShader(m_vertex_shader.Get(), nullptr, 0);
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex_shader{ nullptr };
	};

public:
	/**
	 * Holds matrices necessary for rendering, which are passed to the shader as a uniform.
	 */
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	ShaderProgram() = default;
	ShaderProgram(const ShaderProgram&other) = delete;
	ShaderProgram(ShaderProgram&& other) noexcept = default;
	auto operator=(const ShaderProgram& other) ->ShaderProgram = delete;
	auto operator=(ShaderProgram&& other) ->ShaderProgram & = delete;
	~ShaderProgram() = default;

	void Shutdown();

	auto AddShader(
		ID3D11Device *device, HWND hwnd, ShaderType shader_type, LPCWSTR path
	) -> HRESULT;
	
	auto AddLayout(
		ID3D11Device* device, HWND hwnd, LPCWSTR vs_shader_path
	) -> HRESULT;

	auto AddBuffer(ID3D11Device* device) -> HRESULT;

	auto XM_CALLCONV Render(
		ID3D11DeviceContext *deviceContext,
		const DirectX::FXMMATRIX& worldMatrix,
		const DirectX::CXMMATRIX& viewMatrix,
		const DirectX::CXMMATRIX& projectionMatrix,
		unsigned int indexCount
	) -> HRESULT;

private:
	template <typename T>
	auto CreateShader(
		ID3D11Device* device, HWND hwnd, LPCWSTR shader_path
	) -> HRESULT;

	void RenderShader(ID3D11DeviceContext* deviceContext, unsigned int indexCount);

	static void OutputShaderErrorMessage(
		ID3D10Blob *errorMessage,
		HWND hwnd,
		const WCHAR *shaderFilename
	);

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout{ nullptr };
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_matrix_buffer{ nullptr };

	// This needs to be a vector of pointer because we use inheratence for the shader
	std::vector<std::unique_ptr<Shader>> m_shaders;

};

} // namespace graphics
