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
		pBody->Set_Owner(this);
		m_dwCollisionLayer_Flag = 0UL;
		m_dwCollisionMask_Flag = 0UL;
	}
	FCollisionPrimitive(const FCollisionPrimitive& rhs)
		: m_dwCollisionLayer_Flag(rhs.m_dwCollisionLayer_Flag)
		, m_dwCollisionMask_Flag(rhs.m_dwCollisionMask_Flag)
	{
		pBody = new FRigidBody();
		*pBody = *rhs.pBody;
		pBody->Set_Owner(this);
		//arr = nullptr;		// 이벤트 함수는 복사하면 안됨.
	}
	virtual ~FCollisionPrimitive() 
	{
		Safe_Delete(pBody);
	}
	
public:
	GETSET_EX2(void*, pOwner, Owner, GET, SET)

protected:
	void* pOwner = nullptr;

public:
	FRigidBody* pBody;				// 강체 정보
	
	FMatrix3x4	matOffset;			// 오프셋 행렬

public:
	void Calculate_Transform()
	{
		matTransform = pBody->Get_Transform() * matOffset;
	}

public:
	GETSET_EX2(FMatrix3x4, matTransform, Transform, GET_C_REF, GET_REF)
	// 축 정보를 트랜스폼에서 얻어옴
	FVector3 Get_Axis(_uint iIndex) const
	{
		return matTransform.Get_AxisVector(iIndex);
	}

protected:
	FMatrix3x4	matTransform;		// 트랜스 폼 행렬

public:
	GETSET_EX2(_ulong, m_dwCollisionLayer_Flag, CollisionLayer, GET_C_REF, SET_C)
	GETSET_EX2(_ulong, m_dwCollisionMask_Flag, CollisionMask, GET_C_REF, SET_C)

protected:
	_ulong				m_dwCollisionLayer_Flag;			// 콜리전 레이어, 충돌체가 존재하는 층
	_ulong				m_dwCollisionMask_Flag;				// 콜리전 마스크, 충돌체가 충돌하고 싶어하는 층

public:
	GETSET_EX2(ECOLLISION, eType, Type, GET_C_REF, GET_REF)

protected:
	ECOLLISION	eType;				// 타입

public:
	void Add_CollisionEvent(const function<void(void*)>& fn) { fnEventHandler = fn; }
	void Handle_CollsionEvent(void* pDst) { if (fnEventHandler) fnEventHandler(pDst); }

protected:
	function<void(void*)>	fnEventHandler;
};


/// <summary>
/// 구체
/// </summary>
class ENGINE_DLL FCollisionSphere : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionSphere)

public:
	FCollisionSphere()
		: vPos(0.f, 0.f, 0.f), fRadius(1.f)
	{
		eType = ECOLLISION::SPHERE;
	}
	FCollisionSphere(const FCollisionSphere& rhs)
		: Base(rhs), 
		vPos(rhs.vPos), fRadius(rhs.fRadius)
	{
		eType = ECOLLISION::SPHERE;
	}
	virtual ~FCollisionSphere() {}

public:
	FVector3	vPos;
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
		: vPos(0.f, 0.f, 0.f), vHalfSize(1.f, 1.f, 1.f)
	{
		eType = ECOLLISION::BOX;
	}
	FCollisionBox(const FCollisionBox& rhs)
		: Base(rhs),
		vPos(rhs.vPos), vHalfSize(rhs.vHalfSize)
	{
		eType = ECOLLISION::BOX;
	}
	virtual ~FCollisionBox() {}

public:
	FVector3	vPos;
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
		: vPos(0.f, 0.f, 0.f), vDirHalfSize(0.f, 1.f, 0.f), fRadius(0.5f)
	{
		eType = ECOLLISION::CAPSULE;
	}
	FCollisionCapsule(const FCollisionCapsule& rhs)
		: Base(rhs)
		, vPos(rhs.vPos), vDirHalfSize(rhs.vDirHalfSize), fRadius(rhs.fRadius)
	{
		eType = ECOLLISION::CAPSULE;
	}
	virtual ~FCollisionCapsule() {}

public:
	FVector3	vPos;
	FVector3	vDirHalfSize;
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