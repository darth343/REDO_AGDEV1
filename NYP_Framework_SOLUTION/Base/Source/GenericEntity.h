#ifndef GENERIC_ENTITY_H
#define GENERIC_ENTITY_H

#include "EntityBase.h"
#include <string>
#include "Collider/Collider.h"
#include "LevelOfDetails\LevelOfDetails.h"
#include "SceneGraph\SceneNode.h"

class Mesh;

class GenericEntity : public EntityBase, public CCollider, public CLevelOfDetails
{
public:
	GenericEntity(Mesh* _modelMesh);
	virtual ~GenericEntity();

	virtual void Update(double _dt);
	virtual void Render();
	virtual void Delete();

	virtual Vector3 GetScale();
	void SetAABB(Vector3 maxAABB, Vector3 minAABB);
	float GetHP();
	void SetHP(float health);
	Vector3 GetMin();
	Vector3 GetMax();
	CSceneNode* Body;

private:
	Mesh* modelMesh;
	float hp;
};

namespace Create
{
	GenericEntity* Entity(	const std::string& _meshName, 
							const Vector3& _position, 
							const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f));

	GenericEntity* Entity(const std::string& _meshName,
		const Vector3& _position,
		const Vector3& _scale,
		const bool& collidable);

	GenericEntity* Asset(	const std::string& _meshName,
							const Vector3& _position,
							const Vector3& _scale = Vector3(1.0f, 1.0f, 1.0f));
};

#endif // GENERIC_ENTITY_H