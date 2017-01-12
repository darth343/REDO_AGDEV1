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
	SetAABB(Vector3(8, 8, 8), Vector3(-8, -8, -8));
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

	LeftGun.Init();
	LeftGun.SetFiringRate(30);
	LeftGun.SetMaxTotalRound(9999999);
	LeftGun.SetMaxMagRound(9999999);
	LeftGun.SetMagRound(9999999);
	LeftGun.SetTotalRound(9999999);
	LeftCocked = false;
	LeftDisplacement = 0.f;
	LeftTimer = 0.f;

	RightGun.Init();
	RightGun.SetFiringRate(30);
	RightGun.SetMaxTotalRound(9999999);
	RightGun.SetMaxMagRound(9999999);
	RightGun.SetMagRound(9999999);
	RightGun.SetTotalRound(9999999);
	RightCocked = false;
	RightDisplacement = 0.f;
	RightTimer = 0.f;
	RightGun.SetElapsedTime(-RightGun.GetTimeBetweenShots() * 0.5);
	Body->SetScale(scale.x, scale.y, scale.z);

	isDead = false;
	LRAngle = 0;
	MiddleAngle = 0;
	deathTimer = 0;
	
	SetHP(100);

	EntityManager::GetInstance()->AddEntity(this, true);
}

void CMortar::SetPosition(const Vector3& position)
{
	EntityBase::SetPosition(position);
	Body->Reset();
	HeadMidPart->Body->Reset();
	HeadLeftPart->Body->Reset();
	HeadRightPart->Body->Reset();
	Body->ApplyTranslate(position.x * (1.f / scale.x), position.y * (1.f / scale.y), position.z * (1.f / scale.z));
	HeadMidPart->Body->ApplyTranslate(HeadMidPart->GetPosition().x, HeadMidPart->GetPosition().y, HeadMidPart->GetPosition().z);
	HeadLeftPart->Body->ApplyTranslate(HeadLeftPart->GetPosition().x + LeftDisplacement, HeadLeftPart->GetPosition().y, HeadLeftPart->GetPosition().z);
	HeadRightPart->Body->ApplyTranslate(HeadRightPart->GetPosition().x + RightDisplacement, HeadRightPart->GetPosition().y, HeadRightPart->GetPosition().z);
}

void CMortar::SetIsDone(bool done)
{
	if (done)
	{
		isDead = true;
	}
}

bool CMortar::IsDone()
{
	return EntityBase::IsDone();
}

void CMortar::Reset()
{
}

void CMortar::Update(double dt)
{
	if (isDead)
	{
		MiddleAngle += dt * 200;
		LRAngle += dt * 500;
		deathTimer += dt;
		if (deathTimer > 3)
		{
			EntityBase::SetIsDone(true);
			CSceneGraph::GetInstance()->DeleteNode(BodyPart);
			return;
		}
	}
	if (Body)
	{
		Body->Reset();
		Body->SetScale(scale.x, scale.y, scale.z);
		Body->ApplyTranslate(position.x * (1.f / scale.x), position.y * (1.f / scale.y), position.z * (1.f / scale.z));
		if (HeadMidPart->Body)
		{
			HeadMidPart->Body->Reset();
			HeadMidPart->Body->ApplyTranslate(HeadMidPart->GetPosition().x, HeadMidPart->GetPosition().y, HeadMidPart->GetPosition().z);
			HeadMidPart->Body->ApplyRotate(MiddleAngle, 0, 1, 0);
			if (HeadLeftPart->Body)
			{
				HeadLeftPart->Body->Reset();
				HeadLeftPart->Body->ApplyTranslate(HeadLeftPart->GetPosition().x + LeftDisplacement, HeadLeftPart->GetPosition().y, HeadLeftPart->GetPosition().z);
				HeadLeftPart->Body->ApplyRotate(MiddleAngle, 0, 0, 1);
				if (!isDead)
				{
					LeftGun.Update(dt);
					if (LeftGun.GetCanFire())
					{
						LeftGun.Discharge(position + HeadMidPart->GetPosition() + HeadLeftPart->GetPosition() + Vector3(-80, 0, -5), position + HeadMidPart->GetPosition() + HeadLeftPart->GetPosition() + Vector3(-110, 0, -5));
						LeftGun.Reload();
						LeftDisplacement = 5;
					}
					if (LeftDisplacement > 0)
					{
						LeftDisplacement -= LeftGun.GetTimeBetweenShots() * (5 / LeftGun.GetTimeBetweenShots()) * dt;
						if (LeftDisplacement < 0)
						{
							LeftDisplacement = 0;
						}
					}
				}
			}
			if (HeadRightPart->Body)
			{
				HeadRightPart->Body->Reset();
				HeadRightPart->Body->ApplyTranslate(HeadRightPart->GetPosition().x + RightDisplacement, HeadRightPart->GetPosition().y, HeadRightPart->GetPosition().z);
				HeadRightPart->Body->ApplyRotate(-MiddleAngle, 0, 0, 1);
				if (!isDead)
				{
					RightGun.Update(dt);
					if (RightGun.GetCanFire())
					{
						RightGun.Discharge(position + HeadMidPart->GetPosition() + HeadRightPart->GetPosition() + Vector3(-80, 0, 5), position + HeadMidPart->GetPosition() + HeadRightPart->GetPosition() + Vector3(-110, 0, 5));
						RightGun.Reload();
						RightDisplacement = 5;
					}
					if (RightDisplacement > 0)
					{
						RightDisplacement -= RightGun.GetTimeBetweenShots() * (5 / RightGun.GetTimeBetweenShots()) * dt;
						if (RightDisplacement < 0)
						{
							RightDisplacement = 0;
						}
					}
				}
				//HeadRightPart->Body->ApplyRotate(angle, 0, 0, 1);
			}
		}
	}
}

void CMortar::Render()
{
}