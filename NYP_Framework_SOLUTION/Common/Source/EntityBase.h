#ifndef ENTITY_BASE_H
#define ENTITY_BASE_H

#include "Vector3.h"
#include <string>

class EntityBase
{
public:
	EntityBase();
	virtual ~EntityBase();

	virtual void Update(double _dt);
	virtual void Render();
	virtual void RenderUI();

	inline virtual void SetPosition(const Vector3& _value){ position = _value; };
	inline Vector3 GetPosition(){ return position; };

	inline void SetScale(const Vector3& _value){ scale = _value; };
	inline virtual Vector3 GetScale(){ return scale; };

	virtual bool IsDone();
	virtual void SetIsDone(const bool _value);

	bool HasChildren();
	void SetChildren(bool m_bChildren);
	// Check if this entity has a collider class parent
	virtual bool HasCollider(void) const;
	// Set the flag to indicate if this entity has a collider class parent
	virtual void SetCollider(const bool _value);
	// Set the flag, bLaser
	virtual void SetIsLaser(const bool bLaser);
	// Get the flag, bLaser
	virtual bool GetIsLaser(void) const;
	// Get Min
	virtual Vector3 GetMin();
	// Get Max
	virtual Vector3 GetMax();

	void SetType(std::string v_type);
	std::string GetType();

protected:
	Vector3 position;
	Vector3 scale;

	bool isDone;
	bool m_bCollider;
	bool bLaser;
	bool m_bChildren;
	std::string type;
};

#endif // ENTITY_BASE_H