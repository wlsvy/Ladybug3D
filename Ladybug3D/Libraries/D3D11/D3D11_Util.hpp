#pragma once

namespace Ladybug3D::D3D11 {
	class TextureImpl;

	void* CreateTexture();
	void CreateTexture(TextureImpl & outTexture);

}