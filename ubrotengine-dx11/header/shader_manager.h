///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: shader_manager.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <array>
#include <tuple>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "shader_program.h"


namespace graphics
{
	enum class ShaderProg : uint8_t
	{
		SimShader = 0,
		ColShader,
		TexShader,
		LigShader,
		NomShader,
		TesShader,
		NUMBER
	};

///////////////////////////////////////////////////////////////////////////////////////////////////
// Class name: ShaderManager
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderManager
{

public:
	ShaderManager() = default;
	ShaderManager(const ShaderManager &other) = delete;
	ShaderManager(ShaderManager&& other) noexcept = delete;
	auto operator=(const ShaderManager& other) -> ShaderManager = delete;
	auto operator=(ShaderManager&& other) -> ShaderManager& = delete;
	~ShaderManager() = default;

	auto Initialize(ID3D11Device* device, HWND hwnd) -> HRESULT;
	/**
	 * Shuts down by calling shut down for every member (where possible) and freeing allocated
	 * memory.
	 */
	void Shutdown();

	auto AddCustomShaderProgram() -> std::tuple<size_t, ShaderProgram&>;
	auto GetShaderProgram(size_t shader_prog_idx) -> ShaderProgram&;
	void RemoveShaderProgram(size_t program_idx);

private:
	std::array<std::tuple<ShaderProgram, unsigned int>, uint8_t(ShaderProg::NUMBER)> m_default_shader_progs{};
	std::vector<std::tuple<ShaderProgram, unsigned int>> m_custom_shader_progs{};

};

} // namespace graphics
