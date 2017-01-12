#include "EntityBase.h"

EntityBase::EntityBase() 
	: position(0.0f, 0.0f, 0.0f)
	, scale(1.0f, 1.0f, 1.0f)
	, isDone(false)
	, m_bCollider(false)
	, bLaser(false)
	, m_bChildren(false)
	, type("base")
{
}

EntityBase::~EntityBase()
{
}

void EntityBase::Update(double _dt)
{
}

void EntityBase::Render()
{
}

void EntityBase::RenderUI()
{
}

bool EntityBase::IsDone()
{
	return isDone;
}

void EntityBase::SetIsDone(bool _value)
{
	isDone = _value;
}

// Check if this entity has a collider class parent
bool EntityBase::HasCollider(void) const
{
	return m_bCollider;
}

// Set the flag to indicate if this entity has a collider class parent
void EntityBase::SetCollider(const bool _value)
{
	m_bCollider = _value;
}

// Set the flag, bLaser
void EntityBase::SetIsLaser(const bool bLaser)
{
	this->bLaser = bLaser;
}

// Get the flag, bLaser
bool EntityBase::GetIsLaser(void) const
{
	return bLaser;
}

Vector3 EntityBase::GetMin()
{
	return Vector3(position.x - 0.1, position.y - 0.1, position.z - 0.1);
}

Vector3 EntityBase::GetMax()
{
	return Vector3(position.x + 0.1, position.y + 0.1, position.z + 0.1);
}

bool EntityBase::HasChildren()
{
	return m_bChildren;
}

void EntityBase::SetType(std::string v_type)
{
	type = v_type;
}

std::string EntityBase::GetType()
{
	return type;
}

void EntityBase::SetChildren(bool m_Children)
{
	m_bChildren = m_Children;
}