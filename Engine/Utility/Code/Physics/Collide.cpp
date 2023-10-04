#include "Collide.h"

bool FCollisionDetector::SphereAndSphere(const FCollisionSphere& srcSphere, const FCollisionSphere& dstSphere)
{
    FVector3 vSub = dstSphere.vPos - srcSphere.vPos;
    if (srcSphere.fRadius * srcSphere.fRadius + dstSphere.fRadius * dstSphere.fRadius >= vSub.SquareMagnitude())
        return true;

    return false;
}

bool FCollisionDetector::SphereAndBox(const FCollisionSphere& srcSphere, const FCollisionBox& dstBox)
{
    return false;
}

bool FCollisionDetector::SphereAndPlane(const FCollisionSphere& srcSphere, const FCollisionPlane& dstPlane)
{
    return false;
}

bool FCollisionDetector::BoxAndBox(const FCollisionBox& srcBox, const FCollisionBox& dstBox)
{
    bool bSrcTest = false, bDstTest = false;
    if (srcBox.vPos + srcBox.vHalfSize <= dstBox.vPos - dstBox.vHalfSize)
    {
        bSrcTest = true;
    }

    if (dstBox.vPos + dstBox.vHalfSize <= srcBox.vPos - srcBox.vHalfSize)
    {
        bDstTest = true;
    }

    return (bSrcTest && bDstTest);
}

bool FCollisionDetector::BoxAndPlane(const FCollisionBox& srcBox, const FCollisionPlane& dstPlane)
{
    return false;
}

bool FCollisionDetector::CapapsuleAndBox(const FCollisionCapsule& srcCapsule, const FCollisionBox& dstBox)
{
    return false;
}

bool FCollisionDetector::CapapsuleAndSphere(const FCollisionCapsule& srcCapsule, const FCollisionSphere& dstSphere)
{
    return false;
}

bool FCollisionDetector::CapapsuleAndPlane(const FCollisionCapsule& srcCapsule, const FCollisionPlane& dstPlane)
{
    return false;
}
