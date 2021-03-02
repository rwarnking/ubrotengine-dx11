///////////////////////////////////////////////////////////////////////////////////////////////////
// Filename: graphics_settings.h
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//////////////
// INCLUDES //
//////////////
#include <iostream>
#include <fstream>

///////////////////////
// MY CLASS INCLUDES //
///////////////////////


namespace graphics
{

struct GraphicSettings
{
	BOOL fullscreen{ TRUE };
	bool v_sync{ false };

	float screen_near{ 0.1F };
	float screen_depth{ 100.0F };

	uint16_t window_width{ 1920 };
	uint16_t window_height{ 1080 };

	GraphicSettings() = default;
	GraphicSettings(const GraphicSettings& other) = delete;
	GraphicSettings(GraphicSettings&& other) noexcept = delete;
	auto operator=(const GraphicSettings& other)->GraphicSettings = delete;
	auto operator=(GraphicSettings&& other)->GraphicSettings & = delete;
	~GraphicSettings() = default;

	// https://stackoverflow.com/questions/21344106/serializing-struct-to-file-and-deserializing-it-again-with-string
	// https://stackoverflow.com/questions/12802536/c-multiple-definitions-of-operator
	friend std::ostream& operator<<(std::ostream& os, const graphics::GraphicSettings& settings)
	{
		return os << settings.fullscreen << ' ' << settings.v_sync
			<< ' ' << settings.screen_near << ' ' << settings.screen_depth
			<< ' ' << settings.window_width << ' ' << settings.window_height;
	};

	friend std::istream& operator>>(std::istream& os, graphics::GraphicSettings& settings)
	{
		os >> settings.fullscreen;
		os >> settings.v_sync;
		os >> settings.screen_near;
		os >> settings.screen_depth;
		os >> settings.window_width;
		os >> settings.window_height;
		return os;
	};
};

} // namespace graphics
