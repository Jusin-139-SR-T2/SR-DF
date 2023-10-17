#include "Collide.h"


bool FCollisionDetector::CollsionPrimitive(const FCollisionPrimitive* srcShape, const FCollisionPrimitive* dstShape, FCollisionData* pColData)
{
	bool bCollide = false;
	switch (srcShape->Get_Type())
	{
	case ECOLLISION::SPHERE:
	{
		switch (dstShape->Get_Type())
		{
		case ECOLLISION::SPHERE:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::SphereAndSphere(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::BOX:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::SphereAndBox(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::CAPSULE:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			bCollide = FCollisionDetector::SphereAndCapsule(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::PLANE:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionPlane* pShapeDst = static_cast<const FCollisionPlane*>(dstShape);
			bCollide = FCollisionDetector::SphereAndPlane(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::LINE:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionLine* pShapeDst = static_cast<const FCollisionLine*>(dstShape);
			bCollide = FCollisionDetector::SphereAndLine(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::RAY:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionRay* pShapeDst = static_cast<const FCollisionRay*>(dstShape);
			bCollide = FCollisionDetector::SphereAndRay(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::TRIANGLE:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionTriangle* pShapeDst = static_cast<const FCollisionTriangle*>(dstShape);
			bCollide = FCollisionDetector::SphereAndTriangle(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::OBB:
		{
			const FCollisionSphere* pShapeSrc = static_cast<const FCollisionSphere*>(srcShape);
			const FCollisionOBB* pShapeDst = static_cast<const FCollisionOBB*>(dstShape);
			bCollide = FCollisionDetector::SphereAndOBB(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		}
		break;
	}
	case ECOLLISION::BOX:
	{
		switch (dstShape->Get_Type())
		{
		case ECOLLISION::SPHERE:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::BoxAndSphere(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::BOX:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::BoxAndBox(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::CAPSULE:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			bCollide = FCollisionDetector::BoxAndCapsule(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::PLANE:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionPlane* pShapeDst = static_cast<const FCollisionPlane*>(dstShape);
			bCollide = FCollisionDetector::BoxAndPlane(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::LINE:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionLine* pShapeDst = static_cast<const FCollisionLine*>(dstShape);
			bCollide = FCollisionDetector::BoxAndLine(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::RAY:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionRay* pShapeDst = static_cast<const FCollisionRay*>(dstShape);
			bCollide = FCollisionDetector::BoxAndRay(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::TRIANGLE:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionTriangle* pShapeDst = static_cast<const FCollisionTriangle*>(dstShape);
			bCollide = FCollisionDetector::BoxAndTriangle(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::OBB:
		{
			const FCollisionBox* pShapeSrc = static_cast<const FCollisionBox*>(srcShape);
			const FCollisionOBB* pShapeDst = static_cast<const FCollisionOBB*>(dstShape);
			bCollide = FCollisionDetector::BoxAndOBB(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		}
		break;
	}
	case ECOLLISION::CAPSULE:
	{
		switch (dstShape->Get_Type())
		{
		case ECOLLISION::SPHERE:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndSphere(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::BOX:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndBox(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::CAPSULE:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndCapsule(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::PLANE:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionPlane* pShapeDst = static_cast<const FCollisionPlane*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndPlane(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::LINE:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionLine* pShapeDst = static_cast<const FCollisionLine*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndLine(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::RAY:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionRay* pShapeDst = static_cast<const FCollisionRay*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndRay(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::TRIANGLE:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionTriangle* pShapeDst = static_cast<const FCollisionTriangle*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndTriangle(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::OBB:
		{
			const FCollisionCapsule* pShapeSrc = static_cast<const FCollisionCapsule*>(srcShape);
			const FCollisionOBB* pShapeDst = static_cast<const FCollisionOBB*>(dstShape);
			bCollide = FCollisionDetector::CapsuleAndOBB(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		}
		break;
	}
	case ECOLLISION::OBB:
	{
		switch (dstShape->Get_Type())
		{
		case ECOLLISION::SPHERE:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionSphere* pShapeDst = static_cast<const FCollisionSphere*>(dstShape);
			bCollide = FCollisionDetector::OBBAndSphere(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::BOX:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionBox* pShapeDst = static_cast<const FCollisionBox*>(dstShape);
			bCollide = FCollisionDetector::OBBAndBox(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::CAPSULE:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionCapsule* pShapeDst = static_cast<const FCollisionCapsule*>(dstShape);
			bCollide = FCollisionDetector::OBBAndCapsule(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::PLANE:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionPlane* pShapeDst = static_cast<const FCollisionPlane*>(dstShape);
			bCollide = FCollisionDetector::OBBAndPlane(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::LINE:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionLine* pShapeDst = static_cast<const FCollisionLine*>(dstShape);
			bCollide = FCollisionDetector::OBBAndLine(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::RAY:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionRay* pShapeDst = static_cast<const FCollisionRay*>(dstShape);
			bCollide = FCollisionDetector::OBBAndRay(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::TRIANGLE:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionTriangle* pShapeDst = static_cast<const FCollisionTriangle*>(dstShape);
			bCollide = FCollisionDetector::OBBAndTriangle(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		case ECOLLISION::OBB:
		{
			const FCollisionOBB* pShapeSrc = static_cast<const FCollisionOBB*>(srcShape);
			const FCollisionOBB* pShapeDst = static_cast<const FCollisionOBB*>(dstShape);
			bCollide = FCollisionDetector::OBBAndOBB(*pShapeSrc, *pShapeDst, pColData);
			break;
		}
		}
		break;
	}
	}

	return bCollide;
}

bool FCollisionDetector::SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere, FCollisionData* pColData)
{
	FVector3 vSub = dstSphere.Get_Position() - srcSphere.Get_Position();
	Real fPenetration = srcSphere.fRadius + dstSphere.fRadius - vSub.Magnitude();

	// 충돌정보 생성
	if (pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactNormal = vSub * (((Real)1.0) / vSub.Magnitude());
		pContact.vContactPoint = srcSphere.Get_Position() + vSub * (Real)0.5;
		pContact.fPenetration = fPenetration;
		pContact.Set_BodyData(srcSphere.pBody, dstSphere.pBody, pColData->fFriction, pColData->fRestitution);
	}

	return fPenetration >= 0;
}

bool FCollisionDetector::SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox, FCollisionData* pColData)
{
	FVector3 vResult = srcSphere.Get_Position();
	FVector3 vMin = dstBox.Get_Position() - dstBox.vHalfSize;
	FVector3 vMax = dstBox.Get_Position() + dstBox.vHalfSize;

	vResult.x = (vResult.x < vMin.x) ? vMin.x : vResult.x;
	vResult.y = (vResult.y < vMin.y) ? vMin.y : vResult.y;
	vResult.z = (vResult.z < vMin.z) ? vMin.z : vResult.z;

	vResult.x = (vResult.x > vMax.x) ? vMax.x : vResult.x;
	vResult.y = (vResult.y > vMax.y) ? vMax.y : vResult.y;
	vResult.z = (vResult.z > vMax.z) ? vMax.z : vResult.z;

	FVector3 vClosestPoint = vResult;
	Real fDistSq = (srcSphere.Get_Position() - vClosestPoint).SquareMagnitude();
	Real fRadiusSq = (srcSphere.fRadius * srcSphere.fRadius);

	// 충돌정보 생성
	if (pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactNormal = (vClosestPoint - srcSphere.Get_Position()).Unit();
		pContact.vContactPoint = vClosestPoint;
		pContact.fPenetration = srcSphere.fRadius - real_sqrt(fDistSq);
		pContact.Set_BodyData(srcSphere.pBody, dstBox.pBody, pColData->fFriction, pColData->fRestitution);
	}

	return fDistSq < fRadiusSq;
}

bool FCollisionDetector::SphereAndCapsule(const FCollisionSphere& srcSphere, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
{
	FVector3 vDst_Normal = dstCapsule.vDirHalfSize.Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * dstCapsule.fRadius;
	FVector3 vDst_A = dstCapsule.Get_Position() - dstCapsule.vDirHalfSize + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = dstCapsule.Get_Position() + dstCapsule.vDirHalfSize - vDst_LineEndOffset;			// B 구 위치

	FVector3 vBest = FLineTests::ClosestPointOnLineSegment(vDst_A, vDst_B, srcSphere.Get_Position());

	FVector3 vSub = vBest - srcSphere.Get_Position();
	Real fSqLength = vSub.SquareMagnitude();

	return ((srcSphere.fRadius + dstCapsule.fRadius) * (srcSphere.fRadius + dstCapsule.fRadius) >= fSqLength);
}

bool FCollisionDetector::SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{

	return false;
}

bool FCollisionDetector::SphereAndTriangle(const FCollisionSphere& srcSphere, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::SphereAndOBB(const FCollisionSphere& srcSphere, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	// 
	FVector3 vResult = dstOBB.Get_Position();
	FVector3 vDir = srcSphere.Get_Position() - dstOBB.Get_Position();

	for (int i = 0; i < 3; i++)
	{
		const Real* orientation = &(dstOBB.Get_Transform().data[i * 4]);
		FVector3 vAxis(orientation[0], orientation[1], orientation[2]);
		vAxis.Normalize();

		Real fDistance = vAxis.DotProduct(vDir);

		if (fDistance > dstOBB.vHalfSize.data[i])
			fDistance = dstOBB.vHalfSize.data[i];
		if (fDistance < -dstOBB.vHalfSize.data[i])
			fDistance = -dstOBB.vHalfSize.data[i];

		vResult = vResult + (vAxis * fDistance);
	}

	// 구한 접점을 가지고 거리 체크
	FVector3& vClosestPoint = vResult;
	Real fDistSq = (srcSphere.Get_Position() - vClosestPoint).SquareMagnitude();
	Real fRadiusSq = (srcSphere.fRadius * srcSphere.fRadius);

	// 충돌정보 생성
	if (fDistSq < fRadiusSq
	&& pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactNormal = (vClosestPoint - srcSphere.Get_Position()).Unit();
		pContact.vContactPoint = vClosestPoint;
		pContact.fPenetration = srcSphere.fRadius - real_sqrt(fDistSq);
		pContact.Set_BodyData(srcSphere.pBody, dstOBB.pBody, pColData->fFriction, pColData->fRestitution);
	}
	
	return fDistSq < fRadiusSq;
}

bool FCollisionDetector::BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox, FCollisionData* pColData)
{
	return (((srcBox.Get_Position() + srcBox.vHalfSize) >= (dstBox.Get_Position() - dstBox.vHalfSize))
		&& ((dstBox.Get_Position() + dstBox.vHalfSize) >= (srcBox.Get_Position() - srcBox.vHalfSize)));
}

bool FCollisionDetector::BoxAndCapsule(const FCollisionBox& srcBox, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
{
	// 최적의 구 위치 선별해내기
	FVector3 vDst_Normal = dstCapsule.vDirHalfSize.Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * dstCapsule.fRadius;
	FVector3 vDst_A = dstCapsule.Get_Position() - dstCapsule.vDirHalfSize + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = dstCapsule.Get_Position() + dstCapsule.vDirHalfSize - vDst_LineEndOffset;			// B 구 위치

	FVector3 vBest = FLineTests::ClosestPointOnLineSegment(vDst_A, vDst_B, srcBox.Get_Position());

	// 박스 구 충돌
	FVector3 vResult = vBest;
	FVector3 vMin = srcBox.Get_Position() - srcBox.vHalfSize;
	FVector3 vMax = srcBox.Get_Position() + srcBox.vHalfSize;

	vResult.x = (vResult.x < vMin.x) ? vMin.x : vResult.x;
	vResult.y = (vResult.y < vMin.y) ? vMin.y : vResult.y;
	vResult.z = (vResult.z < vMin.z) ? vMin.z : vResult.z;

	vResult.x = (vResult.x > vMax.x) ? vMax.x : vResult.x;
	vResult.y = (vResult.y > vMax.y) ? vMax.y : vResult.y;
	vResult.z = (vResult.z > vMax.z) ? vMax.z : vResult.z;

	FVector3 vClosestPoint = vResult;
	Real fDistSq = (vBest - vClosestPoint).SquareMagnitude();
	Real fRadiusSq = (dstCapsule.fRadius * dstCapsule.fRadius);

	return fDistSq < fRadiusSq;
}

bool FCollisionDetector::BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::BoxAndTriangle(const FCollisionBox& srcBox, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::BoxAndOBB(const FCollisionBox& srcBox, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::CapsuleAndCapsule(const FCollisionCapsule& srcCapsule, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
{
	FVector3 vSrc_Normal = srcCapsule.vDirHalfSize.Unit();
	FVector3 vSrc_LineEndOffset = vSrc_Normal * srcCapsule.fRadius;
	FVector3 vSrc_A = srcCapsule.Get_Position() - srcCapsule.vDirHalfSize + vSrc_LineEndOffset;			// A 구 위치
	FVector3 vSrc_B = srcCapsule.Get_Position() + srcCapsule.vDirHalfSize - vSrc_LineEndOffset;			// B 구 위치

	FVector3 vDst_Normal = dstCapsule.vDirHalfSize.Unit();
	FVector3 vDst_LineEndOffset = vDst_Normal * dstCapsule.fRadius;
	FVector3 vDst_A = dstCapsule.Get_Position() - dstCapsule.vDirHalfSize + vDst_LineEndOffset;			// A 구 위치
	FVector3 vDst_B = dstCapsule.Get_Position() + dstCapsule.vDirHalfSize - vDst_LineEndOffset;			// B 구 위치

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
	Real fPenetration_Depth = srcCapsule.fRadius + dstCapsule.fRadius - fLength;
	bool bIntersects = fPenetration_Depth > 0.f;

	// 충돌정보 생성
	if (pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactNormal = vPenetration_Normal.Unit();
		pContact.vContactPoint = vBestA + vPenetration_Normal * (Real)0.5;
		pContact.fPenetration = fPenetration_Depth;
		pContact.Set_BodyData(srcCapsule.pBody, dstCapsule.pBody, pColData->fFriction, pColData->fRestitution);
	}

	return bIntersects;
}

bool FCollisionDetector::CapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::CapsuleAndTriangle(const FCollisionCapsule& srcCapsule, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::CapsuleAndOBB(const FCollisionCapsule& srcCapsule, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	//FVector3 vSrc_Normal = srcCapsule.vDirHalfSize.Unit();
	//FVector3 vSrc_LineEndOffset = vSrc_Normal * srcCapsule.fRadius;
	//FVector3 vSrc_A = srcCapsule.Get_Position() - srcCapsule.vDirHalfSize + vSrc_LineEndOffset;			// A 구 위치
	//FVector3 vSrc_B = srcCapsule.Get_Position() + srcCapsule.vDirHalfSize - vSrc_LineEndOffset;			// B 구 위치


	//FVector3 vResult = dstOBB.Get_Position();
	//FVector3 vDir = srcSphere.Get_Position() - dstOBB.Get_Position();

	//for (int i = 0; i < 3; i++)
	//{
	//	const Real* orientation = &(dstOBB.Get_Transform().data[i * 4]);
	//	FVector3 vAxis(orientation[0], orientation[1], orientation[2]);
	//	vAxis.Normalize();

	//	Real fDistance = vAxis.DotProduct(vDir);

	//	if (fDistance > dstOBB.vHalfSize.data[i])
	//		fDistance = dstOBB.vHalfSize.data[i];
	//	if (fDistance < -dstOBB.vHalfSize.data[i])
	//		fDistance = -dstOBB.vHalfSize.data[i];

	//	vResult = vResult + (vAxis * fDistance);
	//}

	//// 구한 접점을 가지고 거리 체크
	//FVector3& vClosestPoint = vResult;
	//Real fDistSq = (srcSphere.Get_Position() - vClosestPoint).SquareMagnitude();
	//Real fRadiusSq = (srcSphere.fRadius * srcSphere.fRadius);

	//// 충돌정보 생성
	//if (fDistSq < fRadiusSq
	//	&& pColData != nullptr && pColData->iContactsLeft >= 0)
	//{
	//	FContact& pContact = pColData->tContacts;
	//	pContact.vContactNormal = (vClosestPoint - srcSphere.Get_Position()).Unit();
	//	pContact.vContactPoint = vClosestPoint;
	//	pContact.fPenetration = srcSphere.fRadius - real_sqrt(fDistSq);
	//	pContact.Set_BodyData(srcSphere.pBody, dstOBB.pBody, pColData->fFriction, pColData->fRestitution);
	//}

	//return fDistSq < fRadiusSq;
	return false;
}

bool FCollisionDetector::PlaneAndPlane(const FCollisionPlane& srcPlane, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndCapsule(const FCollisionLine& srcLine, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndBox(const FCollisionLine& srcLine, const FCollisionBox& dstBox, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndSphere(const FCollisionLine& srcLine, const FCollisionSphere& dstSphere, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndPlane(const FCollisionLine& srcLine, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndLine(const FCollisionLine& srcLine, const FCollisionLine& dstLine, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndRay(const FCollisionLine& srcLine, const FCollisionRay& dstRay, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndTriangle(const FCollisionLine& srcLine, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::LineAndOBB(const FCollisionLine& srcLine, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::RayAndCapsule(const FCollisionRay& srcRay, const FCollisionCapsule& dstCapsule, FCollisionData* pColData)
{
	

	return false;
}

bool FCollisionDetector::RayAndBox(const FCollisionRay& srcRay, const FCollisionBox& dstBox, FCollisionData* pColData)
{
	FVector3 vMin = dstBox.Get_Position() - dstBox.vHalfSize;
	FVector3 vMax = dstBox.Get_Position() + dstBox.vHalfSize;

	// Any component of direction could be 0!
	// Address this by using a small number, close to
	// 0 in case any of directions components are 0
	Real fT1 = (vMin.x - srcRay.vOrigin.x) / (CMP(srcRay.vDir.x, 0.0f) ? 0.00001f : srcRay.vDir.x);
	Real fT2 = (vMax.x - srcRay.vOrigin.x) / (CMP(srcRay.vDir.x, 0.0f) ? 0.00001f : srcRay.vDir.x);
	Real fT3 = (vMin.y - srcRay.vOrigin.y) / (CMP(srcRay.vDir.y, 0.0f) ? 0.00001f : srcRay.vDir.y);
	Real fT4 = (vMax.y - srcRay.vOrigin.y) / (CMP(srcRay.vDir.y, 0.0f) ? 0.00001f : srcRay.vDir.y);
	Real fT5 = (vMin.z - srcRay.vOrigin.z) / (CMP(srcRay.vDir.z, 0.0f) ? 0.00001f : srcRay.vDir.z);
	Real fT6 = (vMax.z - srcRay.vOrigin.z) / (CMP(srcRay.vDir.z, 0.0f) ? 0.00001f : srcRay.vDir.z);

	Real fTmin = fmaxf(fmaxf(fminf(fT1, fT2), fminf(fT3, fT4)), fminf(fT5, fT6));
	Real fTMax = fminf(fminf(fmaxf(fT1, fT2), fmaxf(fT3, fT4)), fmaxf(fT5, fT6));

	// if tmax < 0, ray is intersecting AABB
	// but entire AABB is behing it's origin
	if (fTMax < 0) {
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (fTmin > fTMax) {
		return false;
	}

	float fT_Result = fTmin;

	// If tmin is < 0, tmax is closer
	if (fTmin < 0.0f) {
		fT_Result = fTMax;
	}

	if (pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactPoint = srcRay.vOrigin + srcRay.vDir * fT_Result;
		//pContact.vContactNormal = (pContact.vContactPoint - dstBox.Get_Position()).Unit();
		pContact.fPenetration = 0.f;
		pContact.Set_BodyData(srcRay.pBody, dstBox.pBody, pColData->fFriction, pColData->fRestitution);

		FVector3 normals[] = {
			FVector3(-1, 0, 0),
			FVector3(1, 0, 0),
			FVector3(0, -1, 0),
			FVector3(0, 1, 0),
			FVector3(0, 0, -1),
			FVector3(0, 0, 1)
		};
		float t[] = { fT1, fT2, fT3, fT4, fT5, fT6 };

		for (int i = 0; i < 6; ++i) {
			if (CMP(fT_Result, t[i])) {
				pContact.vContactNormal = normals[i];
			}
		}
	}

	return false;
}

bool FCollisionDetector::RayAndSphere(const FCollisionRay& srcRay, const FCollisionSphere& dstSphere, FCollisionData* pColData)
{
	FVector3 vDstDir = dstSphere.Get_Position() - srcRay.vOrigin;
	Real fRadiusSq = dstSphere.fRadius * dstSphere.fRadius;

	Real fDstDirSq = vDstDir.SquareMagnitude();
	Real fA = srcRay.vDir.DotProduct(vDstDir);
	Real fBSq = fDstDirSq - (fA * fA);
	Real fF = real_sqrt(real_abs(fRadiusSq) - fBSq);

	Real fT = fA - fF;

	if (fRadiusSq - (fDstDirSq - fA * fA) < 0.0f)
	{
		return false;
	}

	if (fDstDirSq < fRadiusSq)
	{
		fT = fA + fF;
	}

	// 충돌정보 생성
	if (fDstDirSq < fRadiusSq
		&& pColData != nullptr && pColData->iContactsLeft >= 0)
	{
		FContact& pContact = pColData->tContacts;
		pContact.vContactPoint = srcRay.vOrigin + srcRay.vDir * fT;
		pContact.vContactNormal = (pContact.vContactPoint - dstSphere.Get_Position()).Unit();
		pContact.fPenetration = 0.f;
		pContact.Set_BodyData(srcRay.pBody, dstSphere.pBody, pColData->fFriction, pColData->fRestitution);
	}

	return true;
}

bool FCollisionDetector::RayAndPlane(const FCollisionRay& srcRay, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::RayAndLine(const FCollisionRay& srcRay, const FCollisionLine& dstLine, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::RayAndRay(const FCollisionRay& srcRay, const FCollisionRay& dstRay, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::RayAndTriangle(const FCollisionRay& srcRay, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::RayAndOBB(const FCollisionRay& srcRay, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::TriangleAndPlane(const FCollisionTriangle& srcTriangle, const FCollisionPlane& dstPlane, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::TriangleAndTriangle(const FCollisionTriangle& srcTriangle, const FCollisionTriangle& dstTriangle, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::TriangleAndOBB(const FCollisionTriangle& srcTriangle, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	return false;
}

bool FCollisionDetector::OBBAndOBB(const FCollisionOBB& srcOBB, const FCollisionOBB& dstOBB, FCollisionData* pColData)
{
	return false;
}
