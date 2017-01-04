#include "Mortar.h"
#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"

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
	SetPosition(Vector3(50, 10, -50));
	SetScale(Vector3(0.2, 0.2, 0.2));

	BodyPart      = new CMortarPart();
	HeadMidPart   = new CMortarPart();
	HeadLeftPart  = new CMortarPart();
	HeadRightPart = new CMortarPart();

	
	BodyPart->InitLOD("MortarBody","MortarBody","MortarBody");
	BodyPart->SetPosition(position);
	BodyPart->SetScale(scale);
	BodyPart->Init();

	HeadMidPart->InitLOD("MortarMidHead", "MortarMidHead", "MortarMidHead");
	HeadMidPart->Init();
	HeadMidPart->SetPosition(Vector3(0, 16, 0));

	HeadLeftPart->InitLOD("MortarLeftHead", "MortarMidHead", "MortarMidHead");
	HeadLeftPart->Init();
	HeadLeftPart->SetPosition(Vector3(0, 0, -18));

	HeadRightPart->InitLOD("MortarRightHead", "MortarRightHead", "MortarRightHead");
	HeadRightPart->Init();
	HeadRightPart->SetPosition(Vector3(0, 0, 18));

	Body = CSceneGraph::GetInstance()->AddNode(BodyPart);
	MidHead = Body->AddChild(HeadMidPart);
	LeftHead = MidHead->AddChild(HeadLeftPart);
	RightHead = MidHead->AddChild(HeadRightPart);

	EntityManager::GetInstance()->AddEntity(this, true);
}

void CMortar::Reset()
{
}

void CMortar::Update(double dt)
{
	Body->Reset();
	RightHead->Reset();
	LeftHead->Reset();
	MidHead->Reset();


	static float angle = -45.f;
	static bool state = false;
	if (!state)
	{
		angle += (float)dt * 20.f;
		if (angle > 45.f)
			state = true;
	}
	else
	{
		angle -= (float)dt * 20.f;
		if (angle < -45.f)
			state = false;
	}

	Body->SetScale(scale.x, scale.y, scale.z);
	Body->ApplyTranslate(position.x * (1.f / scale.x), position.y * (1.f / scale.y), position.z * (1.f / scale.z));
	MidHead->ApplyTranslate(HeadMidPart->GetPosition().x, HeadMidPart->GetPosition().y, HeadMidPart->GetPosition().z);
	MidHead->ApplyRotate(angle, 0, 1, 0);
	RightHead->ApplyTranslate(HeadRightPart->GetPosition().x, HeadRightPart->GetPosition().y, HeadRightPart->GetPosition().z);
	RightHead->ApplyRotate(angle, 0, 0, 1);
	LeftHead->ApplyTranslate(HeadLeftPart->GetPosition().x, HeadLeftPart->GetPosition().y, HeadLeftPart->GetPosition().z);
	LeftHead->ApplyRotate(-angle, 0, 0, 1);
}

void CMortar::Render()
{
}