#include "Mortar.h"
#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "../PlayerInfo/PlayerInfo.h"
#include "../SpatialPartition/SpatialPartition.h"

CMortar::CMortar()
: GenericEntity(NULL)
{
	type = "Mortar";
}

CMortar::~CMortar()
{
}

void CMortar::Init()
{
	SetCollider(true);
	SetAABB(Vector3(20, 20, 20), Vector3(-20, -20, -20));
	SetScale(Vector3(1, 1, 1));
	SetChildren(true);
	this->SetAABB(MeshBuilder::GetInstance()->GetMesh("MortarBody")->Max *2, MeshBuilder::GetInstance()->GetMesh("MortarBody")->Min *2);

	BodyPart      = new CMortarPart();
	HeadMidPart   = new CMortarPart();
	HeadLeftPart  = new CMortarPart();
	HeadRightPart = new CMortarPart();

	
	BodyPart->InitLOD("MortarBody","MortarBody","MortarBody");
	BodyPart->SetPosition(position);
	BodyPart->SetScale(scale);
	BodyPart->SetAABB(MeshBuilder::GetInstance()->GetMesh("MortarBody")->Max, MeshBuilder::GetInstance()->GetMesh("MortarBody")->Min);
	BodyPart->SetChildren(true);
	BodyPart->SetType("main");
	BodyPart->Init();

	HeadMidPart->InitLOD("MortarMidHead", "MortarMidHead", "MortarMidHead");
	HeadMidPart->SetPosition(Vector3(0, 16, 0));
	HeadMidPart->SetAABB(MeshBuilder::GetInstance()->GetMesh("MortarMidHead")->Max, MeshBuilder::GetInstance()->GetMesh("MortarMidHead")->Min);
	HeadMidPart->SetChildren(true);
	HeadMidPart->Init();

	HeadLeftPart->InitLOD("MortarLeftHead", "MortarMidHead", "MortarMidHead");
	HeadLeftPart->SetPosition(Vector3(0, 0, -18));
	HeadLeftPart->SetAABB(MeshBuilder::GetInstance()->GetMesh("MortarLeftHead")->Max, MeshBuilder::GetInstance()->GetMesh("MortarLeftHead")->Min);
	HeadLeftPart->Init();

	HeadRightPart->InitLOD("MortarRightHead", "MortarRightHead", "MortarRightHead");
	HeadRightPart->SetPosition(Vector3(0, 0, 18));
	HeadRightPart->SetAABB(MeshBuilder::GetInstance()->GetMesh("MortarRightHead")->Max, MeshBuilder::GetInstance()->GetMesh("MortarRightHead")->Min);
	HeadRightPart->Init();

	Body = CSceneGraph::GetInstance()->AddNode(BodyPart);
	HeadMidPart->Body   = Body->AddChild(HeadMidPart);
	HeadLeftPart->Body  = HeadMidPart->Body->AddChild(HeadLeftPart);
	HeadRightPart->Body = HeadMidPart->Body->AddChild(HeadRightPart);

	EntityManager::GetInstance()->AddEntity(this, true);
}

void CMortar::Reset()
{
}

void CMortar::Update(double dt)
{
	if (Body)
	{
		Body->Reset();
		Body->SetScale(scale.x, scale.y, scale.z);
		Body->ApplyTranslate(position.x * (1.f / scale.x), position.y * (1.f / scale.y), position.z * (1.f / scale.z));
	}
	if (HeadMidPart->Body)
	{
		HeadMidPart->Body->Reset();
		HeadMidPart->Body->ApplyTranslate(HeadMidPart->GetPosition().x, HeadMidPart->GetPosition().y, HeadMidPart->GetPosition().z);
		HeadMidPart->Body->ApplyRotate(angle, 0, 1, 0);
	}
	if (HeadLeftPart->Body)
	{
		HeadLeftPart->Body->Reset();
		HeadLeftPart->Body->ApplyTranslate(HeadLeftPart->GetPosition().x, HeadLeftPart->GetPosition().y, HeadLeftPart->GetPosition().z);
		//HeadLeftPart->Body->ApplyRotate(-angle, 0, 0, 1);
	}
	if (HeadRightPart->Body)
	{
		HeadRightPart->Body->Reset();
		HeadRightPart->Body->ApplyTranslate(HeadRightPart->GetPosition().x, HeadRightPart->GetPosition().y, HeadRightPart->GetPosition().z);
		//HeadRightPart->Body->ApplyRotate(angle, 0, 0, 1);
	}
}

void CMortar::Render()
{
}