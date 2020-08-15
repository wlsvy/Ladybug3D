#include "D3D11_Common.hpp"
#include <iostream>

using namespace std;

namespace Ladybug3D::D3D11 {

	void ThrowIfFailed(HRESULT hr, const char* msg)
	{
		if (FAILED(hr))
		{
			throw std::exception(msg);
		}
	}

	void ThrowIfFailed(HRESULT hr, const std::string& msg)
	{
		ThrowIfFailed(hr, msg.c_str());
	}

	void PrintConsoleLog(const char* msg)
	{
		cout << msg << endl;
	}

	void PrintConsoleLog(const std::string& msg)
	{
		cout << msg << endl;
	}
}




