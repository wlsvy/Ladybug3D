#pragma once
#include <d3d11.h>
#include <stdint.h>
#include <string>
#include <exception>

namespace Ladybug3D::D3D11 {

	void ThrowIfFailed(HRESULT hr, const char* msg);
	void ThrowIfFailed(HRESULT hr, const std::string& msg);

	void PrintConsoleLog(const char* msg);
	void PrintConsoleLog(const std::string& msg);

	constexpr uint8_t MAX_RENDER_TARGET_BINDING_COUNT = 4;
}