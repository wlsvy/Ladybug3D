#include "D3D11_Resources.hpp"

#include <d3d11.h>
#include "D3D11_Device.hpp"

using namespace std;
using namespace Ladybug3D;
using namespace Ladybug3D::D3D11;
 
Resources::Resources()
{
}

Resources::~Resources()
{
}

bool Resources::Initialize(HWND hwnd, UINT width, UINT height)
{
	m_Device = make_shared<Device>();

	return m_Device->Initialize(hwnd, width, height);
}
