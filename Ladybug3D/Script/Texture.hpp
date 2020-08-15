#pragma once
#include <Object.hpp>

namespace Ladybug3D {
	class TextureImpl;

	class Texture : public Object {
	public:
		Texture();
		~Texture();

		const TextureImpl& Get() const { return *m_Impl; }

	private:
		std::shared_ptr<TextureImpl> m_Impl;
	};
}