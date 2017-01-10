#include "Grid.h"
#include "stdio.h"
#include "MeshBuilder.h"
#include "RenderHelper.h"
#include "../GenericEntity.h"

/********************************************************************************
Constructor
********************************************************************************/
CGrid::CGrid(void)
	: index(Vector3(-1, -1, -1))
	, size(Vector3(-1, -1, -1))
	, offset(Vector3(-1, -1, -1))
	, min(Vector3(-1, -1, -1))
	, max(Vector3(-1, -1, -1))
	, theMesh(NULL)
	, ListOfObjects(NULL)
	, theDetailLevel(CLevelOfDetails::NO_DETAILS)
{
}

/********************************************************************************
Destructor
********************************************************************************/
CGrid::~CGrid(void)
{
	if (theMesh)
	{
		delete theMesh;
		theMesh = NULL;
	}
	Remove();
}

/********************************************************************************
Initialise this grid
********************************************************************************/
void CGrid::Init(	const int xIndex, const int zIndex,
					const int xGridSize, const int zGridSize,
					const float xOffset, const float zOffset)
{
	index.Set(xIndex, 0, zIndex);
	size.Set(xGridSize, 0, zGridSize);
	offset.Set(xOffset, 0, zOffset);
	min.Set(index.x * size.x - offset.x, 0.0f, index.z * size.z - offset.z);
	max.Set(index.x * size.x - offset.x + xGridSize, 0.0f, index.z * size.z - offset.z + zGridSize);
}

/********************************************************************************
 Set a particular grid's Mesh
********************************************************************************/
void CGrid::SetMesh(const std::string& _meshName)
{
	Mesh* modelMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	if (modelMesh != nullptr)
	{
		theMesh = MeshBuilder::GetInstance()->GetMesh(_meshName);
	}
}

/********************************************************************************
Update the grid
********************************************************************************/
void CGrid::Update(vector<EntityBase*>* migrationList)
{
	// Check each object to see if they are no longer in this grid
	std::vector<EntityBase*>::iterator it;
	it = ListOfObjects.begin();
	while (it != ListOfObjects.end())
	{
		if ((*it)->HasCollider())
		{
			Vector3 position = (*it)->GetPosition();
			if (((min.x <= position.x) && (position.x <= max.x)) &&
				((min.z <= position.z) && (position.z <= max.z)))
			{
				// Move on otherwise
				++it;
			}
			else
			{
				migrationList->push_back(*it);

				// Remove from this Grid
				it = ListOfObjects.erase(it);
			}
		}
		else
		{
			Vector3 position = (*it)->GetPosition();
			Vector3 it_Min = (*it)->GetMin();
			Vector3 it_Max = (*it)->GetMax();

			if (it_Min.x > max.x &&
				it_Max.x < min.x &&
				it_Min.z > max.z &&
				it_Max.z < min.z)
			{
				// Move on otherwise
				++it;
			}
			else
			{
				migrationList->push_back(*it);

				// Remove from this Grid
				it = ListOfObjects.erase(it);
			}
		}
	}
}

/********************************************************************************
RenderScene
********************************************************************************/
void CGrid::Render(void)
{
	if (theMesh)
	{
		if (ListOfObjects.size() > 0)
		{
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (ListOfObjects.size() == 1)
				RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("GRIDMESH_GREEN"));
			if (ListOfObjects.size() == 2)
				RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("GRIDMESH_YELLOW"));
			if (ListOfObjects.size() > 2)
				RenderHelper::RenderMesh(MeshBuilder::GetInstance()->GetMesh("GRIDMESH_RED"));
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

/********************************************************************************
RenderObjects
********************************************************************************/
void CGrid::RenderObjects(const int RESOLUTION)
{
	/*
	glPushAttrib(GL_ENABLE_BIT);
	// Draw the Grid and its list of objects
	for (int i=0; i<(int)ListOfObjects.size(); i++)
	{
	ListOfObjects[i]->Render(RESOLUTION);
	}
	glPopAttrib();
	*/
}

/********************************************************************************
Add a new object to this grid
********************************************************************************/
void CGrid::Add(EntityBase* theObject)
{
	for (int i = 0; i < ListOfObjects.size(); ++i)
	{
		if (ListOfObjects[i] == theObject)
			return;
	}
	ListOfObjects.push_back( theObject );
}

/********************************************************************************
 Remove but not delete object from this grid
********************************************************************************/
void CGrid::Remove(void)
{
	for( int i = 0 ; i < ListOfObjects.size(); i++)
	{
		// Do not delete the objects as they are stored in EntityManager and will be deleted there.
		//delete ListOfObjects[i];
		ListOfObjects[i] = NULL;
	}
	ListOfObjects.clear();
}

/********************************************************************************
 Remove but not delete an object from this grid
********************************************************************************/
bool CGrid::Remove(EntityBase* theObject)
{
	// Clean up entities that are done
	std::vector<EntityBase*>::iterator it, end;
	it = ListOfObjects.begin();
	end = ListOfObjects.end();
	while (it != end)
	{
		if ((*it) == theObject)
		{
			it = ListOfObjects.erase(it);
			return true;
		}
		else
		{
			// Move on otherwise
			++it;
		}
	}
	return false;
}

/********************************************************************************
 Check if an object is in this grid
********************************************************************************/
bool CGrid::IsHere(EntityBase* theObject) const
{
	for (int i = 0; i < ListOfObjects.size(); ++i)
	{
		if (ListOfObjects[i] == theObject)
			return true;
	}
	return false;
}

/********************************************************************************
Get list of objects in this grid
********************************************************************************/
vector<EntityBase*> CGrid::GetListOfObject(void)
{
	return ListOfObjects;
}

/********************************************************************************
 PrintSelf
 ********************************************************************************/
void CGrid::PrintSelf()
{
	cout << "CGrid::PrintSelf()" << endl;
	cout << "\tIndex\t:\t" << index << "\t\tOffset\t:\t" << offset << endl;
	cout << "\tMin\t:\t" << min << "\tMax\t:\t" << max << endl;
	cout << "\tList of objects in this grid: (LOD:" << this->theDetailLevel << ")" << endl;
	cout << "\t------------------------------------------------------------------------" << endl;
	if (ListOfObjects.size() > 0)
	{
		for (int i = 0; i < ListOfObjects.size(); ++i)
		{
			cout << "\t" << i << "\t:\t" << ListOfObjects[i]->GetPosition() << endl;
		}
	}
	if (ListOfObjects.size()>0)
		cout << "\t------------------------------------------------------------------------" << endl;
	cout << "********************************************************************************" << endl;
}

/********************************************************************************
 Set the Level of Detail for objects in this CGrid
 ********************************************************************************/
void CGrid::SetDetailLevel(const CLevelOfDetails::DETAIL_LEVEL theDetailLevel)
{
	this->theDetailLevel = theDetailLevel;

	if ((ListOfObjects.size() > 0) && (theDetailLevel == 0))
	{
		// Put a break-point here to trace and see that the entities in this CGrid are set to NO_DETAILS
		int a = 0;
	}
	// Check each object to see if they are no longer in this grid
	std::vector<EntityBase*>::iterator it;
	it = ListOfObjects.begin();
	while (it != ListOfObjects.end())
	{
		GenericEntity* theEntity = (GenericEntity*)(*it);
		if (theEntity->GetLODStatus() == true)
		{
			theEntity->SetDetailLevel(theDetailLevel);
		}
		++it;
	}
}

Vector3 CGrid::GetMax()
{
	return max;
}

Vector3 CGrid::GetMin()
{
	return min;
}