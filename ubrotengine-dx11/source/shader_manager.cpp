///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: shader_manager.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "../header/shader_manager.h"

//////////////
// INCLUDES //
//////////////


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

auto ShaderManager::Initialize(ID3D11Device* device, HWND hwnd) -> HRESULT
{
	auto result{ S_OK };

	size_t idx = 0;
#if _DEBUG
	LPCWSTR vs_path = L"../../engine/ubrotengine-dx11/ubrotengine-dx11/shader/color.vs";
	LPCWSTR fs_path = L"../../engine/ubrotengine-dx11/ubrotengine-dx11/shader/color.fs";
	LPCWSTR layout_path = L"../../engine/ubrotengine-dx11/ubrotengine-dx11/shader/layout.vs";
#else
	LPCWSTR vs_path = L"shader/color.vs";
	LPCWSTR fs_path = L"shader/color.fs";
	LPCWSTR layout_path = L"shader/layout.vs";
#endif
	result = std::get<0>(m_default_shader_progs.at(idx)).AddShader(device, hwnd, ShaderProgram::ShaderType::VertexShader, vs_path);
	if (FAILED(result)) {
		return result;
	}
	result = std::get<0>(m_default_shader_progs.at(idx)).AddShader(device, hwnd, ShaderProgram::ShaderType::FragmentShader, fs_path);
	if (FAILED(result)) {
		return result;
	}
	result = std::get<0>(m_default_shader_progs.at(idx)).AddLayout(device, hwnd, layout_path);
	if (FAILED(result)) {
		return result;
	}
	std::get<0>(m_default_shader_progs.at(idx)).AddBuffer(device);
	idx++;
	if (FAILED(result)) {
		return result;
	}

	// TODO(rwarnking) save layout in the manager and give the shader only an index
#if _DEBUG
	vs_path = L"../../engine/ubrotengine-dx11/ubrotengine-dx11/shader/color.vs";
	fs_path = L"../../engine/ubrotengine-dx11/ubrotengine-dx11/shader/color.fs";
#else
	vs_path = L"shader/color.vs";
	fs_path = L"shader/color.fs";
#endif
	result = std::get<0>(m_default_shader_progs.at(idx)).AddShader(device, hwnd, ShaderProgram::ShaderType::VertexShader, vs_path);
	if (FAILED(result)) {
		return result;
	}
	result = std::get<0>(m_default_shader_progs.at(idx)).AddShader(device, hwnd, ShaderProgram::ShaderType::FragmentShader, fs_path);
	if (FAILED(result)) {
		return result;
	}
	result = std::get<0>(m_default_shader_progs.at(idx)).AddLayout(device, hwnd, layout_path);
	if (FAILED(result)) {
		return result;
	}
	std::get<0>(m_default_shader_progs.at(idx)).AddBuffer(device);
	idx++;
	if (FAILED(result)) {
		return result;
	}
	
	return result;
}


void ShaderManager::Shutdown()
{
	for (auto& s : m_default_shader_progs) {
		std::get<0>(s).Shutdown();
	}
}


auto ShaderManager::AddCustomShaderProgram() -> std::tuple<size_t, ShaderProgram&>
{
	m_custom_shader_progs.emplace_back();
	auto& t = m_custom_shader_progs.back();
	std::get<1>(t)++;
	return std::forward_as_tuple(m_custom_shader_progs.size(), std::get<0>(t));
}


auto ShaderManager::GetShaderProgram(size_t shader_prog_idx) -> ShaderProgram&
{
	if (shader_prog_idx >= size_t(ShaderProg::NUMBER)) {
		auto idx = shader_prog_idx - size_t(ShaderProg::NUMBER);
		return std::get<0>(m_custom_shader_progs.at(idx));
	}
	return std::get<0>(m_default_shader_progs.at(shader_prog_idx));
}


void ShaderManager::RemoveShaderProgram(size_t program_idx)
{
	assert(program_idx < m_default_shader_progs.size() && "RemoveShaderProgram oob");

	auto& t = m_default_shader_progs.at(program_idx);
	if (std::get<1>(t) == 0) {
		std::get<0>(t).Shutdown();
	}
	std::get<1>(t)--;
}

} // namespace graphics
