#include "GenericEntity.h"
#include "MeshBuilder.h"
#include "EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

GenericEntity::GenericEntity(Mesh* _modelMesh)
	: modelMesh(_modelMesh)
	, type("generic")
{
}

GenericEntity::~GenericEntity()
{
}

void GenericEntity::Update(double _dt)
{
	// Does nothing here, can inherit & override or create your own version of this class :D
}

void GenericEntity::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	modelStack.Translate(position.x, position.y, position.z);
	modelStack.Scale(scale.x, scale.y, scale.z);
	if (GetLODStatus()==true)
	{
		if (theDetailLevel != NO_DETAILS)
			RenderHelper::RenderMesh(GetLODMesh());
	}
	else
		RenderHelper::RenderMesh(modelMesh);
	modelStack.PopMatrix();
}

Vector3 GenericEntity::GetMin()
{
	Vector3 min;
	min.x = CCollider::GetMinAABB().x * scale.x + position.x;
	min.y = CCollider::GetMinAABB().y * scale.y + position.y;
	min.z = CCollider::GetMinAABB().z * scale.z + position.z;
	return min;
}

Vector3 GenericEntity::GetMax()
{
	Vector3 max;
	max.x = CCollider::GetMaxAABB().x * scale.x + position.x;
	max.y = CCollider::GetMaxAABB().y * scale.y + position.y;
	max.z = CCollider::GetMaxAABB().z * scale.z + position.z;
	return max;
}

// Set the maxAABB and minAABB
void GenericEntity::SetAABB(Vector3 maxAABB, Vector3 minAABB)
{
	this->maxAABB = maxAABB;
	this->minAABB = minAABB;
}

GenericEntity* Create::Entity(	const std::string& _meshName, 
								const Vector3& _position,
								const Vector3& _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	EntityManager::GetInstance()->AddEntity(result, true);
	return result;
}

GenericEntity* Create::Entity(const std::string& _meshName,
	const Vector3& _position,
	const Vector3& _scale,
	const bool& collidable)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	if (collidable)
	{
		if (modelMesh->Min.IsZero() || modelMesh->Max.IsZero())
			result->SetAABB(Vector3(0.5, 0.5, 0.5), Vector3(-0.5, -0.5, -0.5));
		else
			result->SetAABB(modelMesh->Max, modelMesh->Min);
	}
	result->SetCollider(collidable);
	EntityManager::GetInstance()->AddEntity(result, true);
	return result;
}

GenericEntity* Create::Asset(	const std::string& _meshName,
								const Vector3& _position,
								const Vector3& _scale)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh == nullptr)
		return nullptr;

	GenericEntity* result = new GenericEntity(modelMesh);
	result->SetPosition(_position);
	result->SetScale(_scale);
	result->SetCollider(false);
	return result;
}

