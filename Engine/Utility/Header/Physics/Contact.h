#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "RigidBody.h"

BEGIN(Engine)


/// <summary>
/// ���� ����
/// </summary>
class FContact
{
	THIS_CLASS(FContact)

public:
	FRigidBody* pBody[2];

	Real		fFriction;
	Real		fRestitution;

	FVector3	vContactPoint;
	FVector3	vContactNormal;
};

/// <summary>
/// ���� �ذ�
/// </summary>
class FContactResolver
{
	THIS_CLASS(FContactResolver)

protected:
	_uint	iVelocityIterations;
	_uint	iPositionIterations;
	Real	fVelocityEpsilon;			// �ӵ� ����
	Real	fPositionEpsilon;			// ��ġ ����

public:
	_uint	iVelocityIterationsUsed;
	_uint	iPositionIterationsUsed;

private:
	bool	bValidSettings;				// 

public:
    FContactResolver(_uint iterations, Real velocityEpsilon = (Real)0.01, Real positionEpsilon = (Real)0.01);
    FContactResolver(_uint velocityIterations, _uint positionIterations, Real velocityEpsilon = (Real)0.01, Real positionEpsilon = (Real)0.01);
    
    // ��ȿ üũ
    bool isValid()
    {
        return (iVelocityIterations > 0) &&
            (iPositionIterations > 0) &&
            (fVelocityEpsilon >= 0.0f) &&
            (fPositionEpsilon >= 0.0f);
    }

    // �ݺ� ����
    void Set_Iterations(_uint velocityIterations, _uint positionIterations);
    void Set_Iterations(_uint iterations);
    // ���� ����
    void Set_Epsilon(Real velocityEpsilon,  Real positionEpsilon);
    void ResolveContacts(FContact* contactArray, _uint numContacts, Real duration);

protected:
    // ���� �غ�
    void PrepareContacts(FContact* pContactArray, _uint numContacts, Real duration);
    // �ӵ�, ��ġ ����
    void AdjustVelocities(FContact* pContactArray, _uint numContacts, Real duration);
    void AdjustPositions(FContact* pContacts, _uint numContacts, Real duration);
};


/// <summary>
/// Ư���� �浹�� ���̴� �߻� Ŭ����
/// ���˿� ���� �߻���Ų��.
/// </summary>
class FContactGenerator abstract
{
	THIS_CLASS(FContactGenerator)

public:
	virtual _uint Add_Contact(FContact* pContact, _uint iLimit) const PURE;
};

END