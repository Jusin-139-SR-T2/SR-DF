#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

/// <summary>
/// Ʈ������ ������Ʈ�� ���ӿ�����Ʈ�� �߰��Ǿ� ������Ʈ�� ��ġ������ �����ϴ� ������Ʈ�̴�.
/// SceneComponent������ Ʈ�������� �����Ǳ�� ������ �̴� �� ������Ʈ�� ���� ��ǥ���� ������ ���̸�.
/// �� Ŭ�����ʹ� ����� ����.
/// </summary>
class ENGINE_DLL CTransformComponent : public CGameObjComponent
{
	DERIVED_CLASS(CGameObjComponent, CTransformComponent)

protected:
	explicit CTransformComponent();
	explicit CTransformComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransformComponent(const CTransformComponent& rhs);
	virtual ~CTransformComponent();

public:
	static CTransformComponent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent* Clone();

private:
	virtual void		Free();

public:
	HRESULT			Ready_Transform();
	virtual _int	Update_Component(const _float& fTimeDelta);

	void			Chase_Target(const _vec3* pTargetPos, const _float& fTimeDelta, const _float& fSpeed);
	const _matrix*	Compute_LootAtTarget(const _vec3* pTargetPos);

public:
	// ��ǥ �����Լ�
	void		Move_Pos(const _vec3* const pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}

	// ȸ�� ���� �Լ� (�� 1���� ����)
	void		Rotate(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vRotation) + eType) += fAngle;
	}

	// ���� ����
	void			Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matTransform.m[eType][0], sizeof(_vec3));
	}

	void		Set_WorldMatrixS(const _matrix* pWorld) { m_matTransform = *pWorld; }

	// Set_Pos
	void		Set_Pos(const _float& fX, const _float& fY, const _float& fZ)
	{
		m_vInfo[INFO_POS] = { fX, fY, fZ };
	}

public:		// Ʈ������ ����, Transform���� �Űܿ� ����
	GETSET_EX2(_vec3, m_vInfo[INFO_RIGHT], Right, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_UP], Up, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_LOOK], Look, GET_C_REF, SET_C)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS], Pos, GET_C_REF, SET_C)
	void Set_PosX(const _float value) { m_vInfo[INFO_POS].x = value; }
	void Set_PosY(const _float value) { m_vInfo[INFO_POS].y = value; }
	void Set_PosZ(const _float value) { m_vInfo[INFO_POS].z = value; }


	GETSET_EX2(_vec3, m_vRotation, Rotation, GET_C_REF, SET_C)
	void Set_RotationX(const _float value) { m_vRotation.x = value; }
	void Set_RotationY(const _float value) { m_vRotation.y = value; }
	void Set_RotationZ(const _float value) { m_vRotation.z = value; }

	GETSET_EX2(_vec3, m_vScale, Scale, GET_C_REF, SET_C)
	void Set_ScaleX(const _float value) { m_vScale.x = value; }
	void Set_ScaleY(const _float value) { m_vScale.y = value; }
	void Set_ScaleZ(const _float value) { m_vScale.z = value; }

	GETSET_EX2(_matrix, m_matTransform, Transform, GET_C_PTR, SET_C_PTR)

protected:
	_vec3		m_vInfo[INFO_END];		// ��ġ, ���� ����
	_vec3		m_vRotation;			// ���Ϸ� ȸ����
	_vec3		m_vScale;				// ũ��
	_matrix		m_matTransform;			// ���� ���

};

END