#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "CollisionPrimitive.h"
#include "RigidBody.h"
#include "Contact.h"

BEGIN(Engine)


/// <summary>
/// ���� �������� �������� �浹�׽�Ʈ Ŭ����
/// static Ŭ����, CollsionData�� �����մϴ�.
/// </summary>
class ENGINE_DLL FIntersectTests
{
	THIS_CLASS(FIntersectTests)

public:
	// �� �浹
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// �ڽ� �浹
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere);// { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// ĸ�� �浹
	static bool CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox);
	static bool CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere);
	static bool CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};


/// <summary>
/// �浹�� ���� ������ �����մϴ�.
/// </summary>
struct ENGINE_DLL FCollisionData
{
	THIS_CLASS(FCollisionData)

public:
	FContact*	pContactArray;
	FContact*	pContacts;

    _int		iContactsLeft;

    _uint		iContactCount;

    Real		fFriction;

	Real		fRestitution;
	Real		fTolerance;


    bool HasMoreContacts()
    {
        return iContactsLeft > 0;
    }
    void Reset(unsigned maxContacts)
    {
		iContactsLeft = maxContacts;
		iContactCount = 0;
		pContacts = pContactArray;
    }

	// ���� �߰�
    void AddContacts(unsigned count)
    {
        // Reduce the number of contacts remaining, add number used
		iContactsLeft -= count;
		iContactsLeft += count;

        // Move the array forward
		pContacts += count;
    }
};



/// <summary>
/// �浹�� ���� �����ϴ� Ŭ����
/// IntersectTests�� �ٸ��� �浹�� ���� 
/// </summary>
class ENGINE_DLL FCollisionDetector
{
	THIS_CLASS(FCollisionDetector)

public:
	// �� �浹
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// �ڽ� �浹
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere);// { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// ĸ�� �浹
	static bool CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox);
	static bool CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere);
	static bool CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};

END