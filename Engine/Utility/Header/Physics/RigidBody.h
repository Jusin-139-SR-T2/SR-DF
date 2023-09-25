#pragma once

#include "Engine_Define.h"
#include "PhysicsCore.h"

BEGIN(Engine)

/// <summary>
/// 강체의 물리 속성을 가지는 클래스. 
/// F접두어는 언리얼에서 쓰이는 순수 클래스 접두어다. 
/// 여기서 모티브 받아 붙인 접두어이다.
/// 여기에서 F클래스는 구조체 처럼 쓰이거나 Base를 상속받지 않는 독립적인 형태의 클래스를 제작할 때 쓰인다.
/// Vector 클래스를 FloatVector, Integer, DoubleVector와 구분하기 위해 붙였다고 하는데...그렇다고 한다.
/// 멤버 변수 규칙은 m_를 붙이지 않습니다.
/// </summary>
class FRigidBody
{
	THIS_CLASS(FRigidBody)

public:
    // 질량 함수
    inline void Set_Mass(const Real fMass);
    inline Real Get_Mass() const;
    inline bool hasFiniteMass() const;
    // 역질량 함수
    GETSET_EX2(Real, fInverseMass, InversMass, SET_C, GET)

    // 관성 함수
    inline void Set_InertiaTensor(const FMatrix3& inertiaTensor);
    inline void Get_InertiaTensor(FMatrix3* inertiaTensor) const;
    inline FMatrix3 Get_InertiaTensor() const;
    inline void Get_InertiaTensorWorld(FMatrix3* inertiaTensor) const;
    inline FMatrix3 Get_InertiaTensorWorld() const;
    // 역관성 함수
    inline void Set_InverseInertiaTensor(const FMatrix3& inverseInertiaTensor);
    inline void Get_InverseInertiaTensor(FMatrix3* inverseInertiaTensor) const;
    inline FMatrix3 Get_InverseInertiaTensor() const;
    inline void Get_InverseInertiaTensorWorld(FMatrix3* inverseInertiaTensor) const;
    inline FMatrix3 Get_InverseInertiaTensorWorld() const;


    // 감쇄 함수
    GETSET_EX2(Real, fLinearDamping, LinearDamping, SET_C, GET)
    GETSET_EX2(Real, fAngularDamping, AngularDamping, SET_C, GET)
    inline void Set_Damping(const Real linearDamping, const Real angularDamping);
    

    // 위치 함수
    GETSET_EX2(FVector3, vPosition, Position, SET_C_REF, GET)
    inline void Set_Position(const Real x, const Real y, const Real z);
    inline void Get_Position(FVector3* position) const;


    // 정위 함수
    GETSET_EX1(FQuaternion, qtOrientation, Orientation, GET)
    inline void Set_Orientation(const FQuaternion& orientation);
    inline void Set_Orientation(const Real r, const Real i, const Real j, const Real k);
    inline void Get_Orientation(FQuaternion* orientation) const;
    inline void Get_Orientation(FMatrix3* matrix) const;
    inline void Get_Orientation(Real matrix[9]) const;

    // 트랜스폼 함수
    GETSET_EX1(FMatrix3x4, matTransform, Transform, GET)
    inline void Get_Transform(FMatrix3x4* transform) const;
    inline void Get_Transform(Real matrix[16]) const;
    inline void Get_GLTransform(_float matrix[16]) const;

    // 속도 함수
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


    // 회전 함수
    GETSET_EX2(FVector3, vRotation, Rotation, SET_C_REF, GET)
    inline void Set_Rotation(const Real x, const Real y, const Real z) 
    { vRotation.x = x; vRotation.y = y; vRotation.z = z; }
    inline void Get_Rotation(FVector3* rotation) const 
    { *rotation = vRotation; }
    inline void Add_Rotation(const FVector3& vDeltaRotation) 
    { vRotation += vDeltaRotation; }

    // 동면 함수
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

    // 힘 함수
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

    // 가속 함수
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
    Real            fInverseMass;				    // 역질량
	FMatrix3        matInverseInertiaTensor;		// 역관성텐서
    Real            fLinearDamping;                 // 등속이동 감쇄
    Real            fAngularDamping;                // 각속도 감쇄
    FVector3        vPosition;                      // 위치
    FQuaternion     qtOrientation;                  // 정위, 나중에 쿼터니온으로 바꿀 것
    FVector3        vVelocity;                      // 속도
    FVector3        vRotation;                      // 회전 속도

    FMatrix3        matInverseInertiaTensorWorld;   // 3차원 행렬
    Real            fMotion;                        // 모션
    _bool           bIsAwake;                       // 힘 작용 업데이트 가능 여부
    _bool           bCanSleep;                      // 앱실론 자동 연산 제외 기능
    FMatrix3x4      matTransform;                   // 4차원 행렬, 트랜스폼
    FVector3        vForceAccum;                    // 속도힘 합
    FVector3        vTorqueAccum;                   // 회전힘 합
    FVector3        vAcceleration;                  // 가속도
    FVector3        vLastFrameAcceleration;         // 이전 프레임 가속도


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