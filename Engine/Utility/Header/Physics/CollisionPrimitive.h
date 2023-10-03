#pragma once

#include "Engine_Define.h"

#include "RigidBody.h"

BEGIN(Engine)

enum class ECOLLISION_TYPE
{
	SPHERE, BOX, CAPSULE
};
using ECOLLISION = ECOLLISION_TYPE;


/// <summary>
/// 충돌체
/// 기하학적인 모양을 가지는 클래스
/// 실제 충돌처리 이 클래스를 기반으로 한 파생 클래스를 통해 진행한다.
/// </summary>
class ENGINE_DLL FCollisionPrimitive
{
	THIS_CLASS(FCollisionPrimitive)

public:
	FCollisionPrimitive()
	{
		pBody = new FRigidBody();
	}
	FCollisionPrimitive(const FCollisionPrimitive& rhs)
	{
		pBody = new FRigidBody();
		*pBody = *rhs.pBody;
	}
	~FCollisionPrimitive() 
	{
		Safe_Delete(pBody);
	}
	
	

public:
	FRigidBody* pBody;				// 강체 정보
	

	FMatrix3x4	matOffset;			// 오프셋 행렬

public:
	GETSET_EX1(FMatrix3x4, matTransform, Transform, GET_C_REF)
	// 축 정보를 트랜스폼에서 얻어옴
	FVector3 Get_Axis(_uint iIndex) const
	{
		return matTransform.Get_AxisVector(iIndex);
	}

protected:
	FMatrix3x4	matTransform;		// 트랜스 폼 행렬

public:
	GETSET_EX1(ECOLLISION, eType, Type, GET_C_REF)

protected:
	ECOLLISION	eType;				// 타입

public:
	GETSET_EX1(function<void()>, fnEventHandler, Event, SET)

protected:
	function<void()>	fnEventHandler;
};


/// <summary>
/// 구체
/// </summary>
class ENGINE_DLL FCollisionSphere : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionSphere)

public:
	FCollisionSphere()
	{
		eType = ECOLLISION::SPHERE;
	}
	~FCollisionSphere() {}

public:
	Real		fRadius;
};


/// <summary>
/// 박스
/// </summary>
class ENGINE_DLL FCollisionBox : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionBox)
public:
	FCollisionBox()
	{
		eType = ECOLLISION::BOX;
	}
	~FCollisionBox() {}

public:
	FVector3	vHalfSize;
};


/// <summary>
/// 캡슐
/// </summary>
class ENGINE_DLL FCollisionCapsule : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionCapsule)
public:
	FCollisionCapsule()
	{
		eType = ECOLLISION::CAPSULE;
	}
	~FCollisionCapsule() {}

public:
	FVector3	vStart;
	FVector3	vEnd;
	Real		fRadius;
};


/// <summary>
/// 평면
/// 평면은 강체 정보를 가지지 않는다.
/// </summary>
class ENGINE_DLL FCollisionPlane
{
	THIS_CLASS(FCollisionPlane)

public:
	FVector3	vDirection;
	Real		fOffset;
};

END