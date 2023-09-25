#pragma once

#include "Engine_Define.h"
#include "PhysicsCore.h"

BEGIN(Engine)

/// <summary>
/// ��ü�� ���� �Ӽ��� ������ Ŭ����. 
/// F���ξ�� �𸮾󿡼� ���̴� ���� Ŭ���� ���ξ��. 
/// ���⼭ ��Ƽ�� �޾� ���� ���ξ��̴�.
/// ���⿡�� FŬ������ ����ü ó�� ���̰ų� Base�� ��ӹ��� �ʴ� �������� ������ Ŭ������ ������ �� ���δ�.
/// Vector Ŭ������ FloatVector, Integer, DoubleVector�� �����ϱ� ���� �ٿ��ٰ� �ϴµ�...�׷��ٰ� �Ѵ�.
/// ��� ���� ��Ģ�� m_�� ������ �ʽ��ϴ�.
/// </summary>
class FRigidBody
{
	THIS_CLASS(FRigidBody)

public:
    // ���� �Լ�
    inline void Set_Mass(const Real fMass);
    inline Real Get_Mass() const;
    inline bool hasFiniteMass() const;
    // ������ �Լ�
    GETSET_EX2(Real, fInverseMass, InversMass, SET_C, GET)

    // ���� �Լ�
    inline void Set_InertiaTensor(const FMatrix3& inertiaTensor);
    inline void Get_InertiaTensor(FMatrix3* inertiaTensor) const;
    inline FMatrix3 Get_InertiaTensor() const;
    inline void Get_InertiaTensorWorld(FMatrix3* inertiaTensor) const;
    inline FMatrix3 Get_InertiaTensorWorld() const;
    // ������ �Լ�
    inline void Set_InverseInertiaTensor(const FMatrix3& inverseInertiaTensor);
    inline void Get_InverseInertiaTensor(FMatrix3* inverseInertiaTensor) const;
    inline FMatrix3 Get_InverseInertiaTensor() const;
    inline void Get_InverseInertiaTensorWorld(FMatrix3* inverseInertiaTensor) const;
    inline FMatrix3 Get_InverseInertiaTensorWorld() const;


    // ���� �Լ�
    GETSET_EX2(Real, fLinearDamping, LinearDamping, SET_C, GET)
    GETSET_EX2(Real, fAngularDamping, AngularDamping, SET_C, GET)
    inline void Set_Damping(const Real linearDamping, const Real angularDamping);
    

    // ��ġ �Լ�
    GETSET_EX2(FVector3, vPosition, Position, SET_C_REF, GET)
    inline void Set_Position(const Real x, const Real y, const Real z);
    inline void Get_Position(FVector3* position) const;


    // ���� �Լ�
    GETSET_EX1(FQuaternion, qtOrientation, Orientation, GET)
    inline void Set_Orientation(const FQuaternion& orientation);
    inline void Set_Orientation(const Real r, const Real i, const Real j, const Real k);
    inline void Get_Orientation(FQuaternion* orientation) const;
    inline void Get_Orientation(FMatrix3* matrix) const;
    inline void Get_Orientation(Real matrix[9]) const;

    // Ʈ������ �Լ�
    GETSET_EX1(FMatrix3x4, matTransform, Transform, GET)
    inline void Get_Transform(FMatrix3x4* transform) const;
    inline void Get_Transform(Real matrix[16]) const;
    inline void Get_GLTransform(_float matrix[16]) const;

    // �ӵ� �Լ�
    GETSET_EX2(FVector3, vVelocity, Velocity, SET_C_REF, GET)
    inline void Set_Velocity(const Real x, const Real y, const Real z)
    {
        vVelocity.x = x; vVelocity.y = y; vVelocity.z = z;
    }
    inline void Get_Velocity(FVector3* velocity) const
    {
        *velocity = vVelocity;
    }
    inline void Add_Velocity(const FVector3& vDeltaVelocity)
    {
        vVelocity += vDeltaVelocity;
    }


    // ȸ�� �Լ�
    GETSET_EX2(FVector3, vRotation, Rotation, SET_C_REF, GET)
    inline void Set_Rotation(const Real x, const Real y, const Real z) 
    { vRotation.x = x; vRotation.y = y; vRotation.z = z; }
    inline void Get_Rotation(FVector3* rotation) const 
    { *rotation = vRotation; }
    inline void Add_Rotation(const FVector3& vDeltaRotation) 
    { vRotation += vDeltaRotation; }

    // ���� �Լ�
    inline bool Get_Awake() const { return bIsAwake; }
    inline void Set_Awake(const bool bWake = true)
    {
        if (bWake) {
            bIsAwake = true;

            // Add a bit of motion to avoid it falling asleep immediately.
            fMotion = fSleepEpsilon * 2.0f;
        }
        else {
            bIsAwake = false;
            vVelocity.Clear();
            vRotation.Clear();
        }
    }
    inline bool Get_CanSleep() const { return bCanSleep; }
    inline void Set_CanSleep(const bool _bCanSleep = true)
    {
        bCanSleep = _bCanSleep;
        if (!_bCanSleep && !bIsAwake)
            Set_Awake();
    }

    // �� �Լ�
    inline void Clear_Accumulators()
    {
        vForceAccum.Clear();
        vTorqueAccum.Clear();
    }
    inline void Add_Force(const FVector3& vForce)
    {
        vForceAccum += vForce;
        bIsAwake = true;
    }
    inline void Add_ForceAtPoint(const FVector3& force, const FVector3& point);
    inline void Add_ForceAtBodyPoint(const FVector3& force, const FVector3& point);
    inline void Add_Torque(const FVector3& vTorque)
    {
        vTorqueAccum += vTorque;
        bIsAwake = true;
    }

    // ���� �Լ�
    GETSET_EX2(FVector3, vAcceleration, Acceleration, SET_C_REF, GET)
    inline void Set_Acceleration(const Real x, const Real y, const Real z)
    {
        vAcceleration.x = x; vAcceleration.y = y; vAcceleration.z = z;
    }
    inline void Get_Acceleration(FVector3* pvAcceleration) const
    {
        *pvAcceleration = vAcceleration;
    }

protected:
    Real            fInverseMass;				    // ������
	FMatrix3        matInverseInertiaTensor;		// �������ټ�
    Real            fLinearDamping;                 // ����̵� ����
    Real            fAngularDamping;                // ���ӵ� ����
    FVector3        vPosition;                      // ��ġ
    FQuaternion     qtOrientation;                  // ����, ���߿� ���ʹϿ����� �ٲ� ��
    FVector3        vVelocity;                      // �ӵ�
    FVector3        vRotation;                      // ȸ�� �ӵ�

    FMatrix3        matInverseInertiaTensorWorld;   // 3���� ���
    Real            fMotion;                        // ���
    _bool           bIsAwake;                       // �� �ۿ� ������Ʈ ���� ����
    _bool           bCanSleep;                      // �۽Ƿ� �ڵ� ���� ���� ���
    FMatrix3x4      matTransform;                   // 4���� ���, Ʈ������
    FVector3        vForceAccum;                    // �ӵ��� ��
    FVector3        vTorqueAccum;                   // ȸ���� ��
    FVector3        vAcceleration;                  // ���ӵ�
    FVector3        vLastFrameAcceleration;         // ���� ������ ���ӵ�


public:
    void CalculateDerivedData();
    void Integrate(Real duration);

    
    FVector3 Get_PointInLocalSpace(const FVector3& point) const;
    FVector3 Get_PointInWorldSpace(const FVector3& point) const;
    FVector3 Get_DirectionInLocalSpace(const FVector3& direction) const;
    FVector3 Get_DirectionInWorldSpace(const FVector3& direction) const;
    void Get_LastFrameAcceleration(FVector3* linearAcceleration) const;
    FVector3 Get_LastFrameAcceleration() const;

    

    
    
    
};

END