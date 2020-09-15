#pragma once

namespace Ladybug3D {

	constexpr unsigned int MAX_OBJECT_COUNT = 64;

	struct DescriptorHeapIndex  {
		enum : unsigned int {
			CB_PerScene			= 0,
			CB_PerObject		= CB_PerScene + 1,
			SRV_Skybox			= CB_PerObject + MAX_OBJECT_COUNT,
			Max					= SRV_Skybox + 1,
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