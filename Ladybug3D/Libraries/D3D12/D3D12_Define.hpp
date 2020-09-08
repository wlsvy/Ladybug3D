#pragma once
#include <wrl/client.h>

struct ID3D12Object;
struct IDXGIAdapter;
struct IDXGIAdapter1;
struct IDXGIAdapter2;
struct IDXGIAdapter3;
struct IDXGIAdapter4;
struct IDXGISwapChain3;
struct ID3D12Device;
struct ID3D12Resource;
struct ID3D12CommandAllocator;
struct ID3D12CommandQueue;
struct ID3D12DescriptorHeap;
struct ID3D12PipelineState;
struct ID3D12CommandList;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;
struct IDXGIFactory;
struct IDXGIFactory1;
struct ID3D12PipelineState;
struct ID3D12RootSignature;
struct CD3DX12_CPU_DESCRIPTOR_HANDLE;
struct D3D12_CPU_DESCRIPTOR_HANDLE;
struct CD3DX12_GPU_DESCRIPTOR_HANDLE;
struct D3D12_GPU_DESCRIPTOR_HANDLE;

using SIZE_T = unsigned __int64;
using UINT = unsigned int;
using UINT64 = unsigned long long;
using LPCWSTR = const wchar_t*;

namespace Ladybug3D::D3D12 {
	class Resource;
	class GraphicsCommandList;
	class DescriptorHeapAllocator;
	class Texture;
	class VertexBuffer;
	class IndexBuffer;

	template<typename T>
	class ConstantBuffer;
}