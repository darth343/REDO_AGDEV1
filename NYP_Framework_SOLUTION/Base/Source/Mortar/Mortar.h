#pragma once
#include "../GenericEntity.h"
#include "../GroundEntity.h"
#include "../SceneGraph/SceneGraph.h"
#include "../SceneGraph/SceneNode.h"

#include "MortarPart.h"
#include "../WeaponInfo/Pistol.h"

class Mesh;

class CMortar :
	public GenericEntity
{
protected:
	Vector3 defaultPosition, defaultTarget, defaultUp;
	Vector3 target, up;

	CMortarPart* BodyPart;
	CMortarPart* HeadMidPart;
	CMortarPart* HeadLeftPart;
	CMortarPart* HeadRightPart;

	CPistol LeftGun;
	CPistol RightGun;

	bool LeftCocked;
	bool RightCocked;
	float LeftDisplacement;
	float RightDisplacement;
	float LeftTimer;
	float RightTimer;

	bool isDead;

	float MiddleAngle;
	float LRAngle;
	float deathTimer;

public:

	enum MortarState
	{
		S_STANDBY,
		S_STARTING,
		S_INUSE,
		S_SHUTTINGDOWN,
	};

	CMortar(void);
	virtual ~CMortar();

	virtual void SetPosition(const Vector3& position);
	virtual void SetIsDone(const bool done);
	virtual bool IsDone();

	void Init(void);
	// Reset  instance to default
	void Reset(void);

	void Rotate(bool direction);

	// Update
	void Update(double dt = 0.0333f);

	// Render
	void Render(void);
};
