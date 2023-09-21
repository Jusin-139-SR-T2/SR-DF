#pragma once

#include "Engine_Define.h"

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
    /*void calculateDerivedData();
    void integrate(Real duration);

    void setMass(const Real mass);
    Real getMass() const;
    void setInverseMass(const Real inverseMass);
    Real getInverseMass() const;
    bool hasFiniteMass() const;
    void setInertiaTensor(const Matrix3& inertiaTensor);
    void getInertiaTensor(Matrix3* inertiaTensor) const;
    Matrix3 getInertiaTensor() const;
    void getInertiaTensorWorld(Matrix3* inertiaTensor) const;
    Matrix3 getInertiaTensorWorld() const;
    void setInverseInertiaTensor(const Matrix3& inverseInertiaTensor);
    void getInverseInertiaTensor(Matrix3* inverseInertiaTensor) const;
    Matrix3 getInverseInertiaTensor() const;
    void getInverseInertiaTensorWorld(Matrix3* inverseInertiaTensor) const;
    Matrix3 getInverseInertiaTensorWorld() const;
    void setDamping(const Real linearDamping, const Real angularDamping);
    void setLinearDamping(const Real linearDamping);
    Real getLinearDamping() const;
    void setAngularDamping(const Real angularDamping);
    Real getAngularDamping() const;
    void setPosition(const Vector3& position);
    void setPosition(const Real x, const Real y, const Real z);
    void getPosition(Vector3* position) const;
    Vector3 getPosition() const;
    void setOrientation(const Quaternion& orientation);
    void setOrientation(const Real r, const Real i, const Real j, const Real k);
    void getOrientation(Quaternion* orientation) const;
    Quaternion getOrientation() const;
    void getOrientation(Matrix3* matrix) const;
    void getOrientation(Real matrix[9]) const;
    void getTransform(Matrix4* transform) const;
    void getTransform(Real matrix[16]) const;
    void getGLTransform(float matrix[16]) const;
    Matrix4 getTransform() const;
    Vector3 getPointInLocalSpace(const Vector3& point) const;
    Vector3 getPointInWorldSpace(const Vector3& point) const;
    Vector3 getDirectionInLocalSpace(const Vector3& direction) const;
    Vector3 getDirectionInWorldSpace(const Vector3& direction) const;
    void setVelocity(const Vector3& velocity);
    void setVelocity(const Real x, const Real y, const Real z);
    void getVelocity(Vector3* velocity) const;
    Vector3 getVelocity() const;
    void addVelocity(const Vector3& deltaVelocity);
    void setRotation(const Vector3& rotation);
    void setRotation(const Real x, const Real y, const Real z);
    void getRotation(Vector3* rotation) const;
    Vector3 getRotation() const;
    void addRotation(const Vector3& deltaRotation);
    bool getAwake() const
    {
        return isAwake;
    }
    void setAwake(const bool awake = true);
    bool getCanSleep() const
    {
        return canSleep;
    }
    void setCanSleep(const bool canSleep = true);
    void getLastFrameAcceleration(Vector3* linearAcceleration) const;
    Vector3 getLastFrameAcceleration() const;
    void clearAccumulators();
    void addForce(const Vector3& force);
    void addForceAtPoint(const Vector3& force, const Vector3& point);
    void addForceAtBodyPoint(const Vector3& force, const Vector3& point);
    void addTorque(const Vector3& torque);
    void setAcceleration(const Vector3& acceleration);
    void setAcceleration(const Real x, const Real y, const Real z);
    void getAcceleration(Vector3* acceleration) const;
    Vector3 getAcceleration() const;*/
};

END