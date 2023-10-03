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
	FRigidBody* pBody;				// ��ü ����
	

	FMatrix3x4	matOffset;			// ������ ���

public:
	GETSET_EX1(FMatrix3x4, matTransform, Transform, GET_C_REF)
	// �� ������ Ʈ���������� ����
	FVector3 Get_Axis(_uint iIndex) const
	{
		return matTransform.Get_AxisVector(iIndex);
	}

protected:
	FMatrix3x4	matTransform;		// Ʈ���� �� ���

public:
	GETSET_EX1(ECOLLISION, eType, Type, GET_C_REF)

protected:
	ECOLLISION	eType;				// Ÿ��

public:
	GETSET_EX1(function<void()>, fnEventHandler, Event, SET)

protected:
	function<void()>	fnEventHandler;
};


/// <summary>
/// ��ü
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
/// �ڽ�
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
/// ĸ��
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