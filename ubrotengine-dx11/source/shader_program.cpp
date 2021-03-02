///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: shader.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/shader_program.h"


//////////////
// INCLUDES //
//////////////
#include <array>
#include <d3dcompiler.h>
#include <fstream>
#include <memory>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

void ShaderProgram::Shutdown()
{
	for (auto& s : m_shaders) {
		s.reset();
		m_layout.Reset();
		m_matrix_buffer.Reset();
	}
}

auto ShaderProgram::AddShader(
	ID3D11Device* device, HWND hwnd, ShaderType shader_type, LPCWSTR path
) -> HRESULT
{
	switch (shader_type)
	{
		case ShaderType::VertexShader:
			return CreateShader<ShaderProgram::VertexShader>(device, hwnd, path);
		//case ShaderType::GeometryShader:
		//	return CreateShader<ShaderProgram::GeometryShader>(device, hwnd, path);
		//case ShaderType::HullShader:
		//	return CreateShader<ShaderProgram::HullShader>(device, hwnd, path);
		//case ShaderType::DomainShader:
		//	return CreateShader<ShaderProgram::DomainShader>(device, hwnd, path);
		case ShaderType::FragmentShader:
			return CreateShader<ShaderProgram::PixelShader>(device, hwnd, path);
		//case ShaderType::ComputeShader:
		//	return CreateShader<ShaderProgram::ComputeShader>(device, hwnd, path);
		default:
			return S_FALSE;
	}
}


template <typename T>
auto ShaderProgram::CreateShader(
	ID3D11Device *device, HWND hwnd, LPCWSTR shader_path
) -> HRESULT
{
	HRESULT result{ TRUE };
	Microsoft::WRL::ComPtr<ID3D10Blob> error_message{ nullptr };
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer{ nullptr };

	// Try to compile the fragment shader code given by the file and print an error if it fails
	result = D3DCompileFromFile(
		shader_path, nullptr, nullptr, T::entry_point, T::type,
		D3D10_SHADER_ENABLE_STRICTNESS, 0, shader_buffer.GetAddressOf(), error_message.GetAddressOf());
	if (FAILED(result)) {
		if (error_message != nullptr) {
			OutputShaderErrorMessage(error_message.Get(), hwnd, shader_path);
		}
		else {
			MessageBox(hwnd, shader_path, L"Missing Shader File", MB_OK);
		}

		return result;
	}

	m_shaders.push_back(std::make_unique<T>(T()));
	return m_shaders.back()->Create(device, shader_buffer.Get());
}


auto ShaderProgram::AddLayout(
	ID3D11Device* device, HWND hwnd, LPCWSTR vs_shader_path
) -> HRESULT
{
	auto result{ S_OK };

	std::array<D3D11_INPUT_ELEMENT_DESC, 2> polygonLayout{};
	// Prepare the layout to describe the structure of a vertex. Every attribute translates to
	// a slot whose format and other characteristics have to be set manually.
	// Uses D3D11_APPEND_ALIGNED_ELEMENT to ensure that data blocks are aligned sensibly.
	// The structure here must conform to that in the shader file and the vertex struct.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	Microsoft::WRL::ComPtr<ID3D10Blob> error_message{ nullptr };
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer{ nullptr };
	// Try to compile the vertex shader code given by the file and print an error if it fails
	result = D3DCompileFromFile(
		vs_shader_path, nullptr, nullptr, "LVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, shader_buffer.GetAddressOf(), error_message.GetAddressOf());
	if (FAILED(result)) {
		if (error_message != nullptr) {
			OutputShaderErrorMessage(error_message.Get(), hwnd, vs_shader_path);
		}
		else {
			MessageBox(hwnd, vs_shader_path, L"Missing Shader File", MB_OK);
		}

		return result;
	}

	// Create the input layout and store it in m_layout, using the previously filled description
	result = device->CreateInputLayout(
		polygonLayout.data(), numElements,
		shader_buffer->GetBufferPointer(),
		shader_buffer->GetBufferSize(), m_layout.GetAddressOf()
	);
	return result;
}


// TODO(rwarnking) change to variable buffer
auto ShaderProgram::AddBuffer(ID3D11Device* device) -> HRESULT
{
	D3D11_BUFFER_DESC buffer_desc;

	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.ByteWidth = sizeof(MatrixBufferType);
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;

	return device->CreateBuffer(&buffer_desc, nullptr, m_matrix_buffer.GetAddressOf());
}


// in generell smarter attribute/buffer setup?
auto XM_CALLCONV ShaderProgram::Render(
	ID3D11DeviceContext *deviceContext,
	const DirectX::FXMMATRIX& worldMatrix,
	const DirectX::CXMMATRIX& viewMatrix,
	const DirectX::CXMMATRIX& projectionMatrix,
	unsigned int indexCount
) -> HRESULT
{
	auto result{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Matrices must be transposed (because DirectX 11)
	const DirectX::XMMATRIX worldMatrixTransposed = XMMatrixTranspose(worldMatrix);
	const DirectX::XMMATRIX viewMatrixTransposed = XMMatrixTranspose(viewMatrix);
	const DirectX::XMMATRIX projectionMatrixTransposed = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(
		m_matrix_buffer.Get(),
		0, D3D11_MAP_WRITE_DISCARD,
		0, &mappedResource
	);
	if (FAILED(result)) {
		return result;
	}

	auto *data_ptr1 = static_cast<MatrixBufferType*>(mappedResource.pData);
	data_ptr1->world = worldMatrixTransposed;
	data_ptr1->view = viewMatrixTransposed;
	data_ptr1->projection = projectionMatrixTransposed;

	deviceContext->Unmap(m_matrix_buffer.Get(), 0);

	// Choose the buffer number to be used in the shader and set the buffer
	unsigned int bufferNumber = 0;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, m_matrix_buffer.GetAddressOf());


	// Call render to draw
	RenderShader(deviceContext, indexCount);

	return result;
}


void ShaderProgram::RenderShader(ID3D11DeviceContext *deviceContext, unsigned int indexCount)
{
	deviceContext->IASetInputLayout(m_layout.Get());

	for (auto& s : m_shaders) {
		s->Set(deviceContext);
	}

	deviceContext->DrawIndexed(indexCount, 0, 0);
}


void ShaderProgram::OutputShaderErrorMessage(
	ID3D10Blob *errorMessage,
	HWND hwnd,
	const WCHAR *shaderFilename
)
{
	// Prepare error messages
	size_t buffer_size = errorMessage->GetBufferSize();
	std::string err_string(static_cast<char*>(errorMessage->GetBufferPointer()), buffer_size);

	// Open the file to write to
	std::ofstream fout;
	fout.open("shader-error.txt");
	fout << err_string;
	fout.close();

	// Open a message box to signal to the user that an error has occurred and can be
	// inspected in the previously written file.
	MessageBox(
		hwnd,
		L"Error compiling shader.  Check shader-error.txt for message.",
		shaderFilename,
		MB_OK
	);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//template bool
//Shader::CreateShader<Shader::PixelShader>(
//	ID3D11Device *device, HWND hwnd, LPCWSTR shader_path, ID3D11PixelShader **shader
//);

template HRESULT
ShaderProgram::CreateShader<ShaderProgram::PixelShader>(
	ID3D11Device* device, HWND hwnd, LPCWSTR shader_path
);
template HRESULT
ShaderProgram::CreateShader<ShaderProgram::VertexShader>(
	ID3D11Device* device, HWND hwnd, LPCWSTR shader_path
);

} // namespace graphics
