#pragma once
#include <stdexcept>
#include <Windows.h>
#include <iostream>
#include "D3D12_Define.hpp"

namespace Ladybug3D::D3D12 {

    std::string HrToString(HRESULT hr);

    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr)) {
            throw std::runtime_error(HrToString(hr));
        }
    }
    inline void ThrowIfFailed(HRESULT hr, const char* messageOnFail, const char* messageOnSuccess = nullptr)
    {
        if (FAILED(hr)) {
            throw std::runtime_error(messageOnFail);
        }
        else if(messageOnSuccess != nullptr) {
            std::cout << messageOnSuccess << std::endl;
        }
    }


	// Assign a name to the object to aid with debugging.
#if defined(_DEBUG) || defined(DBG)
    void SetName(ID3D12Object* pObject, LPCWSTR name);
    void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index);
#else
    void SetName(ID3D12Object* , LPCWSTR );
    inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT);
#endif

#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)
}