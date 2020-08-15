#pragma once
#include <memory>
#include <string>
#include <Util.hpp>

namespace Ladybug3D {
	class Object : public std::enable_shared_from_this<Object> {
	public:
		Object();
		Object(const std::string& name);
		Object(const Object&);
		Object(Object&&) noexcept;
		virtual ~Object();
		Object& operator=(const Object&);

		uint GetId() const				 { return m_Id; }
		std::shared_ptr<Object> GetPtr() { return shared_from_this(); }
			
		std::string Name = "Object";
	private:
		uint m_Id = -1;
	};
}