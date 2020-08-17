#include "D3D12_Util.hpp"

namespace Ladybug3D::D3D12 {

	std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
		return std::string(s_str);
	}
}