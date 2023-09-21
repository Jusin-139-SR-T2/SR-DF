#pragma once

#include "Engine_Define.h"

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