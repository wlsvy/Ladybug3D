#pragma once
#include <wrl/client.h>
#include <string>

struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

namespace Ladybug3D::D3D11 {
	class TextureImpl {
	public:
		TextureImpl();
		TextureImpl(const std::string & filePath);
		~TextureImpl();

		ID3D11ShaderResourceView *			GetTextureResourceView()		const { return m_ShaderResourceView.Get(); }
		ID3D11ShaderResourceView * const *	GetTextureResourceViewAddress()	const { return m_ShaderResourceView.GetAddressOf(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UnorderedResourceView;
	};
}