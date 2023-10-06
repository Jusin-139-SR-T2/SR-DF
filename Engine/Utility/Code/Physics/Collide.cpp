#include "Collide.h"


bool FCollisionDetector::CollsionPrimitive(const FCollisionPrimitive* srcShape, const FCollisionPrimitive* dstShape, FCollisionData* pColData)
{
	bool bCollide = false;
	switch (srcShape->Get_Type())
	{
	case ECOLLISION::SPHERE:
	{
		if (dstShape->Get_Type() == ECOLLISION::SPHERE)
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::SphereAndSphere(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::BOX)
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::SphereAndBox(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::CAPSULE)
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			FCollisionDetector::SphereAndCapsule(*pShapeSrc, *pShapeDst);
		}
		break;
	}
	case ECOLLISION::BOX:
	{
		if (dstShape->Get_Type() == ECOLLISION::SPHERE)
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::BoxAndSphere(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::BOX)
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::BoxAndBox(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::CAPSULE)
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			FCollisionDetector::BoxAndCapsule(*pShapeSrc, *pShapeDst);
		}
		break;
	}
	case ECOLLISION::CAPSULE:
	{
		if (dstShape->Get_Type() == ECOLLISION::SPHERE)
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndSphere(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::BOX)
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndBox(*pShapeSrc, *pShapeDst);
		}
		else if (dstShape->Get_Type() == ECOLLISION::CAPSULE)
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			FCollisionDetector::CapsuleAndCapsule(*pShapeSrc, *pShapeDst);
		}
		break;
	}
	}

	return bCollide;
}

bool FCollisionDetector::SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere)
{
	FVector3 vSub = dstSphere.vPos - srcSphere.vPos;
	return (srcSphere.fRadius * srcSphere.fRadius + dstSphere.fRadius * dstSphere.fRadius >= vSub.SquareMagnitude());
}

bool FCollisionDetector::SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox)
{
	FVector3 vResult = srcSphere.vPos;
	FVector3 vMin = dstBox.vPos - dstBox.vHalfSize;
	FVector3 vMax = dstBox.vPos + dstBox.vHalfSize;

	vResult.x = (vResult.x < vMin.x) ? vMin.x : vResult.x;
	vResult.y = (vResult.y < vMin.y) ? vMin.y : vResult.y;
	vResult.z = (vResult.z < vMin.z) ? vMin.z : vResult.z;

	vResult.x = (vResult.x > vMax.x) ? vMax.x : vResult.x;
	vResult.y = (vResult.y > vMax.y) ? vMax.y : vResult.y;
	vResult.z = (vResult.z > vMax.z) ? vMax.z : vResult.z;

	FVector3 vClosestPoint = vResult;
	Real fDistSq = (srcSphere.vPos - vClosestPoint).SquareMagnitude();
	Real fRadiusSq = (srcSphere.fRadius * srcSphere.fRadius);

	return fDistSq < fRadiusSq;
}

bool FCollisionDetector::SphereAndCapsule(const FCollisionSphere& srcSphere, const FCollisionCapsule& dstCapsule)
{
	FVector3 vDst_Normal = dstCapsule.vDirHalfSize.Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * dstCapsule.fRadius;
	FVector3 vDst_A = dstCapsule.vPos - dstCapsule.vDirHalfSize + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = dstCapsule.vPos + dstCapsule.vDirHalfSize - vDst_LineEndOffset;			// B 구 위치

	FVector3 vBest = FLineTests::ClosestPointOnLineSegment(vDst_A, vDst_B, srcSphere.vPos);

	FVector3 vSub = vBest - srcSphere.vPos;
	Real fSqLength = vSub.Magnitude();

	return (srcSphere.fRadius + dstCapsule.fRadius >= fSqLength);
}

bool FCollisionDetector::SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane)
{

	return false;
}

bool FCollisionDetector::BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox)
{
	return (((srcBox.vPos + srcBox.vHalfSize) >= (dstBox.vPos - dstBox.vHalfSize))
		&& ((dstBox.vPos + dstBox.vHalfSize) >= (srcBox.vPos - srcBox.vHalfSize)));
}

bool FCollisionDetector::BoxAndCapsule(const FCollisionBox& srcBox, const FCollisionCapsule& dstCapsule)
{
	return false;
}

bool FCollisionDetector::BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane)
{
	return false;
}

bool FCollisionDetector::CapsuleAndCapsule(const FCollisionCapsule& srcCapsule, const FCollisionCapsule& dstCapsule)
{
	FVector3 vSrc_Normal = srcCapsule.vDirHalfSize.Unit();
	FVector3 vSrc_LineEndOffset = vSrc_Normal * srcCapsule.fRadius;
	FVector3 vSrc_A = srcCapsule.vPos - srcCapsule.vDirHalfSize + vSrc_LineEndOffset;			// A 구 위치
	FVector3 vSrc_B = srcCapsule.vPos + srcCapsule.vDirHalfSize - vSrc_LineEndOffset;			// B 구 위치

	FVector3 vDst_Normal = dstCapsule.vDirHalfSize.Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * dstCapsule.fRadius;
	FVector3 vDst_A = dstCapsule.vPos - dstCapsule.vDirHalfSize + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = dstCapsule.vPos + dstCapsule.vDirHalfSize - vDst_LineEndOffset;			// B 구 위치

	FVector3 vAA = vDst_A - vSrc_A;
	FVector3 vAB = vDst_A - vSrc_B;
	FVector3 vBA = vDst_B - vSrc_A;
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
	Real fPenetration_Depth = srcCapsule.fRadius + dstCapsule.fRadius - fLength;
	bool bIntersects = fPenetration_Depth > 0.f;

	return bIntersects;
}

bool FCollisionDetector::CapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane)
{
	return false;
}