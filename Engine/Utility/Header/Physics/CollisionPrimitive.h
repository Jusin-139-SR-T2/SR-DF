#pragma once

#include "Engine_Define.h"

#include "RigidBody.h"

BEGIN(Engine)

/// <summary>
/// �浹ü
/// ���������� ����� ������ Ŭ����
/// ���� �浹ó�� �� Ŭ������ ������� �� �Ļ� Ŭ������ ���� �����Ѵ�.
/// </summary>
class FCollisionPrimitive
{
	THIS_CLASS(FCollisionPrimitive)
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


protected:
	function<void()>	fnEventHandler;
};


/// <summary>
/// ��ü
/// </summary>
class FCollisionSphere : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionSphere)

public:
	Real		fRadius;
};


/// <summary>
/// �ڽ�
/// </summary>
class FCollisionBox : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionBox)

public:
	FVector3	vHalfSize;
};


/// <summary>
/// ĸ��
/// </summary>
class FCollisionCapsule : public FCollisionPrimitive
{
	DERIVED_CLASS(FCollisionPrimitive, FCollisionCapsule)

public:
	FVector3	vStart;
	FVector3	vEnd;
	Real		fRadius;
};


/// <summary>
/// ���
/// ����� ��ü ������ ������ �ʴ´�.
/// </summary>
class FCollisionPlane
{
	THIS_CLASS(FCollisionPlane)

public:
	FVector3	vDirection;
	Real		fOffset;
};

END