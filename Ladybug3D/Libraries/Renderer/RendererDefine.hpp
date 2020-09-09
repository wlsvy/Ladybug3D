#pragma once

namespace Ladybug3D {

	constexpr unsigned int MAX_OBJECT_COUNT = 64;

	struct DescriptorHeapIndex  {
		enum : unsigned int {
			CB_PerScene			= 0,
			CB_PerObject		= CB_PerScene + 1,
			Max					= CB_PerObject + MAX_OBJECT_COUNT,
		};
	};

	struct RootSignatureIndex {
		enum : unsigned int {
			CB_PerScene,
			CB_PerObject,
			Max,
		};
	};
	
}