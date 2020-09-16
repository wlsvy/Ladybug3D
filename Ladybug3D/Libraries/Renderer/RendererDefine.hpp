#pragma once

namespace Ladybug3D {

	constexpr unsigned int MAX_OBJECT_COUNT = 64;

	struct ResourceDescriptorIndex  {
		enum : unsigned int {
			CB_PerScene			= 0,
			CB_PerObject		= CB_PerScene + 1,
			SRV_SampleTexture	= CB_PerObject + MAX_OBJECT_COUNT,
			SRV_Skybox			= SRV_SampleTexture + 1,
			Max					= SRV_Skybox + 1,
		};
	};

	struct SamplerDescriptorIndex {
		enum : unsigned char {
			Sample,
			Max,
		};
	};

	struct RootSignatureIndex {
		enum : unsigned char {
			CB_PerScene,
			CB_PerObject,
			SRV_Texture,
			Max,
		};
	};

	struct RenderTargetIndex {
		enum : unsigned char {
			G_Buffer,
			Max
		};
	};
	
}