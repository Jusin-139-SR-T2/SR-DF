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

class ENGINE_DLL FLineTests
{
public:
	inline static FVector3 ClosestPointOnLineSegment(const FVector3& vA, const FVector3& vB, const FVector3& vPoint);
};

inline FVector3 FLineTests::ClosestPointOnLineSegment(const FVector3& vA, const FVector3& vB, const FVector3& vPoint)
{
	FVector3 vAB = vB - vA;
	Real fSaturate = min(max(0, (vPoint - vA).DotProduct(vAB) / vAB.DotProduct(vAB)), 1);

	return vA + vAB * fSaturate;
}

/// <summary>
/// 충돌에 대한 정보를 저장합니다.
/// </summary>
struct ENGINE_DLL FCollisionData
{
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
/// IntersectTests와 다르게 충돌에 대해 이벤트를 작동시킨다.
/// </summary>
class ENGINE_DLL FCollisionDetector
{
public:
	// 구 충돌
	static bool SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere);
	static bool SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox);
	static bool SphereAndCapsule(const FCollisionSphere& srcCapsule, const FCollisionCapsule& dstCapsule);
	static bool SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane);

	// 박스 충돌
	static bool BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox);
	static bool BoxAndSphere(const FCollisionBox& srcBox, const FCollisionSphere& dstSphere) { return SphereAndBox(dstSphere, srcBox); }
	static bool BoxAndCapsule(const FCollisionBox& srcBox, const FCollisionCapsule& dstCapsule);
	static bool BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane);

	// 캡슐 충돌
	static bool CapsuleAndCapsule(const FCollisionCapsule& srcCapsule, const FCollisionCapsule& dstCapsult);
	static bool CapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox) { return BoxAndCapsule(dstBox, srcCapsule); }
	static bool CapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere) { return SphereAndCapsule(dstSphere, srcCapsule); }
	static bool CapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane);
};


END