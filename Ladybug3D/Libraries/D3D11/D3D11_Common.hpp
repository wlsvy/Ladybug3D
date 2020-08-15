#pragma once
#include <d3d11.h>
#include <string>
#include <exception>

namespace Ladybug3D::D3D11 {

	void ThrowIfFailed(HRESULT hr, const char* msg);
	void ThrowIfFailed(HRESULT hr, const std::string& msg);

	void PrintConsoleLog(const char* msg);
	void PrintConsoleLog(const std::string& msg);
}