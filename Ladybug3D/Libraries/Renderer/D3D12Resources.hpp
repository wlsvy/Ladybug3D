//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <D3D12/d3dx12.h>

#include <string>
#include <wrl.h>
#include <shellapi.h>

class D3D12Resources
{
public:
    D3D12Resources();
    virtual ~D3D12Resources();

    UINT GetWidth() const           { return m_width; }
    UINT GetHeight() const          { return m_height; }

protected:
    void GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
        bool requestHighPerformanceAdapter = false);

    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    bool m_useWarpDevice;
};
