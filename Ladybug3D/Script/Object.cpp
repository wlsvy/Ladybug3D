#pragma once
#include "Object.hpp"

using namespace Ladybug3D;

uint GetNewObjectID()
{
	static uint s_Id = 0;
	return s_Id++;
}


Object::Object() : m_Id(GetNewObjectID())
{
}

Object::Object(const std::string& name) : 
	Name(name),
	m_Id(GetNewObjectID())
{
}

Object::Object(const Object& obj) : 
	Name(obj.Name),
	m_Id(GetNewObjectID())
{
}

Object::Object(Object&& obj) noexcept :
	Name(std::move(obj.Name)),
	m_Id(std::move(obj.m_Id))
{
}

Ladybug3D::Object::~Object()
{
}

Object& Object::operator=(const Object& obj)
{
	Name = obj.Name;
	return *this;
}