#pragma once
#include <memory>
#include <string>

namespace Ladybug3D {
	class Object : public std::enable_shared_from_this<Object> {
	public:
		Object();
		Object(const std::string& name);
		Object(const Object&);
		Object(Object&&) noexcept;
		virtual ~Object();
		virtual Object& operator=(const Object&);
		virtual Object& operator=(Object&&) noexcept;

		unsigned int GetId() const			{ return m_Id; }
		std::shared_ptr<Object> GetPtr()	{ return shared_from_this(); }
		
		virtual void OnAwake() {}
		virtual void OnUpdate() {}
		virtual void OnDestroy() {}
		virtual void OnImGui() {}
			
		std::string Name = "Object";
	private:
		unsigned int m_Id = -1;
	};
}