#include "Collide.h"


bool FCollisionDetector::SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere)
{
	FVector3 vSub = dstSphere.vPos - srcSphere.vPos;
	return (srcSphere.fRadius * srcSphere.fRadius + dstSphere.fRadius * dstSphere.fRadius >= vSub.SquareMagnitude());
}

bool FCollisionDetector::SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox)
{

	return false;
}

bool FCollisionDetector::SphereAndCapsule(const FCollisionSphere& srcCapsule, const FCollisionCapsule& dstCapsule)
{

	return false;
}

bool FCollisionDetector::SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane)
{

	return false;
}

bool FCollisionDetector::BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox)
{
	return ((srcBox.vPos + srcBox.vHalfSize <= dstBox.vPos - dstBox.vHalfSize)
		&& (dstBox.vPos + dstBox.vHalfSize <= srcBox.vPos - srcBox.vHalfSize));
}

bool FCollisionDetector::BoxAndCapsule(const FCollisionBox& srcBox, const FCollisionCapsule& dstCapsule)
{
	return false;
}

bool FCollisionDetector::BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane)
{
	return false;
}

bool FCollisionDetector::CapsuleAndCapsule(const FCollisionCapsule& srcCapsule, const FCollisionCapsule& dstCapsult)
{
	FVector3 vSrc_Normal = (srcCapsule.vEnd - srcCapsule.vStart).Unit();
	FVector3 vSrc_LineEndOffset = vSrc_Normal * srcCapsule.fRadius;
	FVector3 vSrc_A = srcCapsule.vStart + vSrc_LineEndOffset;			// A 구 위치
	FVector3 vSrc_B = srcCapsule.vEnd - vSrc_LineEndOffset;				// B 구 위치

	FVector3 vDst_Normal = (srcCapsule.vEnd - srcCapsule.vStart).Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * srcCapsule.fRadius;
	FVector3 vDst_A = srcCapsule.vStart + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = srcCapsule.vEnd - vDst_LineEndOffset;				// B 구 위치

	FVector3 vAA = vDst_A - vSrc_A;
	FVector3 vAB = vDst_B - vSrc_B;
	FVector3 vBA = vDst_A - vSrc_A;
	FVector3 vBB = vDst_B - vSrc_B;

	Real fAA = vAA.SquareMagnitude();
	Real fAB = vAB.SquareMagnitude();
	Real fBA = vBA.SquareMagnitude();
	Real fBB = vBB.SquareMagnitude();


	// A와 B중에 가장 가까운 구체의 점을 선택한다.
	FVector3 vBestA;
	if (fBA < fAA || fBA < fAB || fBB < fAA || fBB < fAB)
		vBestA = vSrc_B;
	else
		vBestA = vSrc_A;

	// 캡슐 Dst의 두점과 Src의 최적의 점을 가지고 계산
	FVector3 vBestB = FLineTests::ClosestPointOnLineSegment(vDst_A, vDst_B, vBestA);

	// 반대의 경우도 계산
	vBestA = FLineTests::ClosestPointOnLineSegment(vSrc_A, vSrc_B, vBestB);

	FVector3 vPenetration_Normal = vBestA - vBestB;
	Real fLength = vPenetration_Normal.Magnitude();
	vPenetration_Normal /= fLength;
	Real fPenetration_Depth = srcCapsule.fRadius + dstCapsult.fRadius - fLength;
	bool bIntersects = fPenetration_Depth > 0.f;

	return bIntersects;
}

bool FCollisionDetector::CapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane)
{
	return false;
}