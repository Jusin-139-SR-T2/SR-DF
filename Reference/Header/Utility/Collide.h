#pragma once

#include "Engine_Define.h"

#include "PhysicsCore.h"
#include "PhysicsPrecision.h"
#include "CollisionPrimitive.h"
#include "RigidBody.h"
#include "Contact.h"

BEGIN(Engine)


/// <summary>
/// 각종 기하학적 모형들의 충돌테스트 클래스
/// static 클래스, CollsionData를 생성합니다.
/// </summary>
class ENGINE_DLL FIntersectTests
{
	THIS_CLASS(FIntersectTests)

public:
	// 구 충돌
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// 박스 충돌
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere);// { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// 캡슐 충돌
	static bool CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox);
	static bool CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere);
	static bool CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};


/// <summary>
/// 충돌에 대한 정보를 저장합니다.
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

	// 접촉 추가
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
/// 충돌에 대해 감지하는 클래스
/// IntersectTests와 다르게 충돌에 대해 
/// </summary>
class ENGINE_DLL FCollisionDetector
{
	THIS_CLASS(FCollisionDetector)

public:
	// 구 충돌
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// 박스 충돌
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere);// { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// 캡슐 충돌
	static bool CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox);
	static bool CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere);
	static bool CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};

END