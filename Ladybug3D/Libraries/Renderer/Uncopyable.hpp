#pragma once

namespace Ladybug3D {
	class Uncopyable {
	public:
		Uncopyable(const Uncopyable&) = delete;
		Uncopyable& operator=(const Uncopyable&) = delete;

	protected:
		Uncopyable() = default;
		Uncopyable(Uncopyable&&) noexcept = default;
		virtual ~Uncopyable() = default;

		Uncopyable& operator=(Uncopyable&&) noexcept = default;
	};
}