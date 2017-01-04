#include "MortarPart.h"
#include "MeshBuilder.h"
#include "../EntityManager.h"
#include "GraphicsManager.h"
#include "RenderHelper.h"
#include "MeshBuilder.h"

CMortarPart::CMortarPart()
: GenericEntity(NULL)
{
	type = "MortarPart";
}

CMortarPart::~CMortarPart()
{
}

void CMortarPart::Init()
{
	//EntityManager::GetInstance()->AddEntity(this, true);
}

void CMortarPart::Reset()
{
}

void CMortarPart::Update(double dt)
{
}

void CMortarPart::Render()
{
	MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	modelStack.PushMatrix();
	if (GetLODStatus() == true)
	{
		if (theDetailLevel != NO_DETAILS)
		{
			RenderHelper::RenderMesh(GetLODMesh());
		}
	}
	modelStack.PopMatrix();
}