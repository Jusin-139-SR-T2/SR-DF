#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "RigidBody.h"

BEGIN(Engine)


/// <summary>
/// 접촉 보고서
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
/// 접촉 해결
/// </summary>
class FContactResolver
{
	THIS_CLASS(FContactResolver)

protected:
	_uint	iVelocityIterations;
	_uint	iPositionIterations;
	Real	fVelocityEpsilon;			// 속도 오차
	Real	fPositionEpsilon;			// 위치 오차

public:
	_uint	iVelocityIterationsUsed;
	_uint	iPositionIterationsUsed;

private:
	bool	bValidSettings;				// 

public:
    FContactResolver(_uint iterations, Real velocityEpsilon = (Real)0.01, Real positionEpsilon = (Real)0.01);
    FContactResolver(_uint velocityIterations, _uint positionIterations, Real velocityEpsilon = (Real)0.01, Real positionEpsilon = (Real)0.01);
    
    // 유효 체크
    bool isValid()
    {
        return (iVelocityIterations > 0) &&
            (iPositionIterations > 0) &&
            (fVelocityEpsilon >= 0.0f) &&
            (fPositionEpsilon >= 0.0f);
    }

    // 반복 세팅
    void Set_Iterations(_uint velocityIterations, _uint positionIterations);
    void Set_Iterations(_uint iterations);
    // 오차 세팅
    void Set_Epsilon(Real velocityEpsilon,  Real positionEpsilon);
    void ResolveContacts(FContact* contactArray, _uint numContacts, Real duration);

protected:
    // 접촉 준비
    void PrepareContacts(FContact* pContactArray, _uint numContacts, Real duration);
    // 속도, 위치 조절
    void AdjustVelocities(FContact* pContactArray, _uint numContacts, Real duration);
    void AdjustPositions(FContact* pContacts, _uint numContacts, Real duration);
};


/// <summary>
/// 특수한 충돌에 쓰이는 추상 클래스
/// 접촉에 대해 발생시킨다.
/// </summary>
class FContactGenerator abstract
{
	THIS_CLASS(FContactGenerator)

public:
	virtual _uint Add_Contact(FContact* pContact, _uint iLimit) const PURE;
};

END