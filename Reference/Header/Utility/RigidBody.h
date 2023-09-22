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


protected:
	_real       fInverseMass;				    // ������
	_matrix     fInverseInertiaTensor;		    // �������ټ�
    _real       fLinearDamping;                 // ����̵� ����
    _real       fAngularDamping;                // ���ӵ� ����
    _vec3       vPosition;                      // ��ġ
    _matrix     qtOrientation;                  // ����, ���߿� ���ʹϿ����� �ٲ� ��
    _vec3       vVelocity;                      // �ӵ�
    _vec3       vRotation;                      // ȸ�� �ӵ�

    _matrix     matInverseInertiaTensorWorld;   // 3���� ���
    _real       fMotion;                        // ���
    _bool       bIsAwake;                       // �� �ۿ� ������Ʈ ���� ����
    _bool       bCanSleep;                      // �۽Ƿ� �ڵ� ���� ���� ���
    _matrix     transformMatrix;                // 4���� ���, Ʈ������
    _vec3       vForceAccum;                    // �ӵ��� ��
    _vec3       vTorqueAccum;                   // ȸ���� ��
    _vec3       vAcceleration;                  // ���ӵ�
    _vec3       vLastFrameAcceleration;         // ���� ������ ���ӵ�


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