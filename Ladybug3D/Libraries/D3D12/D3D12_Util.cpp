#include "D3D12_Util.hpp"
#include "D3D12_Define.hpp"
#include <d3d12.h>

namespace Ladybug3D::D3D12 {

	std::string HrToString(HRESULT hr)
	{
		char s_str[64] = {};
		sprintf_s(s_str, "HRESULT of 0x%08X  ", static_cast<UINT>(hr));
		return std::string(s_str);
	}

#if defined(_DEBUG) || defined(DBG)
	void SetName(ID3D12Object* pObject, LPCWSTR name) {
		pObject->SetName(name);
	}
	void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index) {
		WCHAR fullName[50];
		if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
		{
			pObject->SetName(fullName);
		}
	}
#else
	void SetName(ID3D12Object*, LPCWSTR)
	{
	}
	void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
	{
	}
#endif

}