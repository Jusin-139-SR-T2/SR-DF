#include "RigidBody.h"


void FRigidBody::CalculateDerivedData()
{
	// ȸ�� ���ʹϾ� ����ȭ
	qtOrientation.Normalise();

	// ��ü�� ���� Ʈ������ ���
	_CalculateTransformMatrix(matTransform, vPosition, qtOrientation);

	// ���� �����̽����� ���� �ټ��� ���
	_TransformInertiaTensor(matInverseInertiaTensorWorld, qtOrientation,
								matInverseInertiaTensor, matTransform);
}

void FRigidBody::Integrate(const Real& fDuration)
{
	if (!bIsAwake) return;
	if (!(eBodyType == ERIGID_BODY_TYPE::DYNAMIC)) return;		// ���̳����� �ƴϸ� ���⼭ ���� �ΰ����� ����.

	// ���� ���� ���� ���ӿ� ���ϱ�
	vLastFrameAcceleration = vAcceleration;
	vLastFrameAcceleration.Add_ScaledVector(vForceAccum, fInverseMass);

	// �� ���ӵ��� ���
	FVector3 vAngularAcceleration = matInverseInertiaTensorWorld.Transform(vTorqueAccum);

	// �ӵ� ����
	// ���� �ӵ��� ���ӵ��� ������κ��� ������Ʈ �Ѵ�.
	vVelocity.Add_ScaledVector(vLastFrameAcceleration, fDuration);

	// �� �ӵ��� ���ӵ��� ������κ��� ������Ʈ �Ѵ�.
	vRotation.Add_ScaledVector(vAngularAcceleration, fDuration);

	// �巡�� ���� �ΰ��Ѵ�.
	vVelocity *= real_pow(fLinearDamping, fDuration);
	vRotation *= real_pow(fAngularDamping, fDuration);

	// ��ġ�� �����Ѵ�.
	// ���� ��ġ�� ������Ʈ �Ѵ�.
	vPosition.Add_ScaledVector(vVelocity, fDuration);

	// �� ��ġ�� ������Ʈ �Ѵ�.
	qtOrientation.Add_ScaledVector(vRotation, fDuration);

	// ������ ����ȭ, ���ο� ����� ������Ʈ�Ѵ�.
	// ��ġ, ����
	CalculateDerivedData();

	// �� �ջ����� �����Ѵ�.
	Clear_Accumulators();

	// � ������ ������ ������Ʈ, ��ü�� ��� �� ������ ����.
	if (bCanSleep)
	{
		// �� ���͵��� ������ ��
		Real fCurrentMotion = vVelocity.DotProduct(vVelocity) + vRotation.DotProduct(vRotation);

		// �����̰� �ִ����� ���� ��ġ�� ���
		Real fBias = real_pow(0.5, fDuration);
		fMotion = (fBias * fMotion) + ((1 - fBias) * fCurrentMotion);

		// ������ ����ؼ� ������ ����� ����
		//if (fMotion < fSleepEpsilon) { Set_Awake(false); }
		//else if (fMotion > 10 * fSleepEpsilon) { fMotion = 10 * fSleepEpsilon; }
	}
}
