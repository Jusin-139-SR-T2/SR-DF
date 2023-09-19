#pragma once

#include "GameObjComponent.h"

BEGIN(Engine)

/// <summary>
/// 트랜스폼 컴포넌트는 게임오브젝트에 추가되어 오브젝트의 위치정보를 저장하는 컴포넌트이다.
/// SceneComponent에서도 트랜스폼이 제공되기는 하지만 이는 그 컴포넌트를 위한 좌표값을 가지는 것이며.
/// 이 클래스와는 상관이 없다.
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
	// 좌표 변경함수
	void		Move_Pos(const _vec3* const pDir, const _float& fTimeDelta, const _float& fSpeed)
	{
		m_vInfo[INFO_POS] += *pDir * fTimeDelta * fSpeed;
	}
	// 회전 변경 함수 (축 1개만 가능)
	void		Rotation(ROTATION eType, const _float& fAngle)
	{
		*(((_float*)&m_vAngle) + eType) += fAngle;
	}
	// 벡터 복사
	void			Get_Info(INFO eType, _vec3* pInfo)
	{
		memcpy(pInfo, &m_matWorld.m[eType][0], sizeof(_vec3));
	}
	_matrix* Get_WorldMatrix() { return &m_matWorld; }

	// Set_Pos
	void		Set_Pos(const _float& fX,
		const _float& fY,
		const _float& fZ)
	{
		m_vInfo[INFO_POS] = { fX, fY, fZ };
	}

	void		Set_Pos(const _vec3& vPos)
	{
		m_vInfo[INFO_POS] = vPos;
	}

	_vec3		Get_Pos()
	{
		return m_vInfo[INFO_POS];
	}

public:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;

};

END