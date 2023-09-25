#include "RigidBody.h"
#include <memory.h>
#include <assert.h>

void FRigidBody::Set_Mass(const Real fMass)
{
    assert(fMass != 0);
    fInverseMass = ((Real)1.0) / fMass;
}

Real FRigidBody::Get_Mass() const
{
    if (fInverseMass == 0) {
        return REAL_MAX;
    }
    else {
        return ((Real)1.0) / fInverseMass;
    }
}

bool FRigidBody::hasFiniteMass() const
{
    return fInverseMass >= 0.0f;
}

void FRigidBody::Set_InertiaTensor(const FMatrix3& inertiaTensor)
{
    matInverseInertiaTensor.Set_Inverse(inertiaTensor);
    //_checkInverseInertiaTensor(inverseInertiaTensor);
}

void FRigidBody::Get_InertiaTensor(FMatrix3* inertiaTensor) const
{
}

FMatrix3 FRigidBody::Get_InertiaTensor() const
{
    FMatrix3 it;
    Get_InertiaTensor(&it);
    return it;
}

void FRigidBody::Get_InertiaTensorWorld(FMatrix3* inertiaTensor) const
{
    inertiaTensor->Set_Inverse(matInverseInertiaTensor);
}

FMatrix3 FRigidBody::Get_InertiaTensorWorld() const
{
    return FMatrix3();
}
