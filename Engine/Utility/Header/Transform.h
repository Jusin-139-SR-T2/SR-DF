#pragma once

#include "ActorComponent.h"

BEGIN(Engine)

/// <summary>
/// Ʈ������ ������Ʈ�� ���ӿ�����Ʈ�� �߰��Ǿ� ������Ʈ�� ��ġ������ �����ϴ� ������Ʈ�̴�.
/// SceneComponent������ Ʈ�������� �����Ǳ�� ������ �̴� �� ������Ʈ�� ���� ��ǥ���� ������ ���̸�.
/// �� Ŭ�����ʹ� ����� ����.
/// </summary>
class ENGINE_DLL CTransform : public CActorComponent
{
	DERIVED_CLASS(CActorComponent, CTransform)

protected:
	explicit CTransform();
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform();

public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphicDev);
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
	void		Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
	}
	// ���� ����
	void			Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
	}
	_matrix* Get_WorldMatrix() { return &m_matWorld; }

public:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;

};

END