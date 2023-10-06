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
/// �浹ü
/// ���������� ����� ������ Ŭ����
/// ���� �浹ó�� �� Ŭ������ ������� �� �Ļ� Ŭ������ ���� �����Ѵ�.
/// </summary>
class ENGINE_DLL FCollisionPrimitive
{
	THIS_CLASS(FCollisionPrimitive)

public:
	FCollisionPrimitive()
	{
		pBody = new FRigidBody();
		pBody->Set_Owner(this);
	}
	FCollisionPrimitive(const FCollisionPrimitive& rhs)
	{
		pBody = new FRigidBody();
		*pBody = *rhs.pBody;
		pBody->Set_Owner(this);
		fnEventHandler = nullptr;		// �̺�Ʈ �Լ��� �����ϸ� �ȵ�.
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
	FRigidBody* pBody;				// ��ü ����
	
	FMatrix3x4	matOffset;			// ������ ���

public:
	void Calculate_Transform()
	{
		matTransform = pBody->Get_Transform() * matOffset;
	}

public:
	GETSET_EX2(FMatrix3x4, matTransform, Transform, GET_C_REF, GET_REF)
	// �� ������ Ʈ���������� ����
	FVector3 Get_Axis(_uint iIndex) const
	{
		return matTransform.Get_AxisVector(iIndex);
	}

protected:
	FMatrix3x4	matTransform;		// Ʈ���� �� ���

public:
	GETSET_EX2(ECOLLISION, eType, Type, GET_C_REF, GET_REF)

protected:
	ECOLLISION	eType;				// Ÿ��

public:
	GETSET_EX1(function<void(void*)>, fnEventHandler, Event, SET)
	void Handle_Event(void* pDst) { if (fnEventHandler) fnEventHandler(pDst); }

protected:
	function<void(void*)>	fnEventHandler;
};


/// <summary>
/// ��ü
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
/// �ڽ�
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
/// ĸ��
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
/// ���
/// ����� ��ü ������ ������ �ʴ´�.
/// </summary>
class ENGINE_DLL FCollisionPlane
{
	THIS_CLASS(FCollisionPlane)

public:
	FVector3	vDirection;
	Real		fOffset;
};

END