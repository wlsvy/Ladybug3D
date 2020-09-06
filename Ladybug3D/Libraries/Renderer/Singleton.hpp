#pragma once
#include "Uncopyable.hpp"
#include <memory>

namespace Ladybug3D {

	template<typename T>
	class Singleton : public Uncopyable {
	public:
		static T& GetInstance() {
			static T* s_Instance = nullptr;

			if (s_Instance == nullptr) {
				s_Instance = new T();
				atexit([]()
				{
					delete s_Instance;
				});
			}

			return *s_Instance;
		}

	protected:
		Singleton() = default;
		virtual ~Singleton() = default;
	};
}