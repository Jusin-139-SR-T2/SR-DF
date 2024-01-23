#include "CollisionPrimitive.h"

_bool FBoundingBox::Overlaps(const FBoundingBox* pOther) const
{
    return (vMin > pOther->vMax && pOther->vMin > vMax);
}

Real FBoundingBox::Get_Growth(const FBoundingBox& other) const
{
    return (vMax - vMin).SquareMagnitude() - (other.vMax - other.vMin).SquareMagnitude();
}
