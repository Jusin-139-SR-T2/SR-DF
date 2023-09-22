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


protected:
	_real       fInverseMass;				    // 역질량
	_matrix     fInverseInertiaTensor;		    // 역관성텐서
    _real       fLinearDamping;                 // 등속이동 감쇄
    _real       fAngularDamping;                // 각속도 감쇄
    _vec3       vPosition;                      // 위치
    _matrix     qtOrientation;                  // 정위, 나중에 쿼터니온으로 바꿀 것
    _vec3       vVelocity;                      // 속도
    _vec3       vRotation;                      // 회전 속도

    _matrix     matInverseInertiaTensorWorld;   // 3차원 행렬
    _real       fMotion;                        // 모션
    _bool       bIsAwake;                       // 힘 작용 업데이트 가능 여부
    _bool       bCanSleep;                      // 앱실론 자동 연산 제외 기능
    _matrix     transformMatrix;                // 4차원 행렬, 트랜스폼
    _vec3       vForceAccum;                    // 속도힘 합
    _vec3       vTorqueAccum;                   // 회전힘 합
    _vec3       vAcceleration;                  // 가속도
    _vec3       vLastFrameAcceleration;         // 이전 프레임 가속도


public:
    void calculateDerivedData();
    void integrate(Real duration);

    void Set_Mass(const Real mass);
    Real Get_Mass() const;
    void Set_InverseMass(const Real inverseMass);
    Real Get_InverseMass() const;
    bool hasFiniteMass() const;
    void Set_InertiaTensor(const Matrix3& inertiaTensor);
    void Get_InertiaTensor(Matrix3* inertiaTensor) const;
    Matrix3 Get_InertiaTensor() const;
    void Get_InertiaTensorWorld(Matrix3* inertiaTensor) const;
    Matrix3 Get_InertiaTensorWorld() const;
    void Set_InverseInertiaTensor(const Matrix3& inverseInertiaTensor);
    void Get_InverseInertiaTensor(Matrix3* inverseInertiaTensor) const;
    Matrix3 Get_InverseInertiaTensor() const;
    void Get_InverseInertiaTensorWorld(Matrix3* inverseInertiaTensor) const;
    Matrix3 Get_InverseInertiaTensorWorld() const;
    void Set_Damping(const Real linearDamping, const Real angularDamping);
    void Set_LinearDamping(const Real linearDamping);
    Real Get_LinearDamping() const;
    void Set_AngularDamping(const Real angularDamping);
    Real Get_AngularDamping() const;
    void Set_Position(const FVector3& position);
    void Set_Position(const Real x, const Real y, const Real z);
    void Get_Position(FVector3* position) const;
    FVector3 Get_Position() const;
    void Set_Orientation(const FQuaternion& orientation);
    void Set_Orientation(const Real r, const Real i, const Real j, const Real k);
    void Get_Orientation(FQuaternion* orientation) const;
    FQuaternion Get_Orientation() const;
    void Get_Orientation(Matrix3* matrix) const;
    void Get_Orientation(Real matrix[9]) const;
    void Get_Transform(FMatrix3x4* transform) const;
    void Get_Transform(Real matrix[16]) const;
    void Get_GLTransform(float matrix[16]) const;
    FMatrix3x4 Get_Transform() const;
    FVector3 Get_PointInLocalSpace(const FVector3& point) const;
    FVector3 Get_PointInWorldSpace(const FVector3& point) const;
    FVector3 Get_DirectionInLocalSpace(const FVector3& direction) const;
    FVector3 Get_DirectionInWorldSpace(const FVector3& direction) const;
    void Set_Velocity(const FVector3& velocity);
    void Set_Velocity(const Real x, const Real y, const Real z);
    void Get_Velocity(FVector3* velocity) const;
    FVector3 Get_Velocity() const;
    void addVelocity(const FVector3& deltaVelocity);
    void Set_Rotation(const FVector3& rotation);
    void Set_Rotation(const Real x, const Real y, const Real z);
    void Get_Rotation(FVector3* rotation) const;
    FVector3 Get_Rotation() const;
    void addRotation(const FVector3& deltaRotation);
    bool Get_Awake() const
    {
        return bIsAwake;
    }
    void Set_Awake(const bool awake = true);
    bool Get_CanSleep() const
    {
        return bCanSleep;
    }
    void Set_CanSleep(const bool canSleep = true);
    void Get_LastFrameAcceleration(FVector3* linearAcceleration) const;
    FVector3 Get_LastFrameAcceleration() const;
    void clearAccumulators();
    void addForce(const FVector3& force);
    void addForceAtPoint(const FVector3& force, const FVector3& point);
    void addForceAtBodyPoint(const FVector3& force, const FVector3& point);
    void addTorque(const FVector3& torque);
    void Set_Acceleration(const FVector3& acceleration);
    void Set_Acceleration(const Real x, const Real y, const Real z);
    void Get_Acceleration(FVector3* acceleration) const;
    FVector3 Get_Acceleration() const;
};

END