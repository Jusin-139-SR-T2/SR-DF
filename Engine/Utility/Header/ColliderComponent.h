#pragma once

#include "SceneComponent.h"

#include "PhysicsMgr.h"

BEGIN(Engine)

/// <summary>
/// 콜라이더 컴포넌트는 충돌체 정보와 트랜스폼 정보를 저장하는 클래스입니다.
/// </summary>
class ENGINE_DLL CColliderComponent abstract : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CColliderComponent)

protected:
	explicit CColliderComponent();
	explicit CColliderComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColliderComponent(const CColliderComponent& rhs);
	virtual ~CColliderComponent();

public:
	virtual CComponent*			Clone() PURE;
	virtual void				Free();

public:
	virtual HRESULT	Ready_Collider() PURE;
	virtual _int	Update_Component(const _float& fTimeDelta) { return 0; }
	virtual void	LateUpdate_Component() {}

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX2(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_UP],		Up,		GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_LOOK],	Look,	GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET, SET)

	GETSET_EX2(_vec3, m_vScale,			Scale,			GET, SET)
	GETSET_EX2(_vec3, m_vAngle,			Angle,			GET, SET)
	GETSET_EX2(_matrix, m_matWorld,		WorldMatrix,	GET, SET)

protected:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;


public:		// 충돌체 저장용 포인터
	GETSET_EX2(FPHY_Shape*, m_pShape, Shape, GET, SET)
	
protected:
	FPHY_Shape* m_pShape = nullptr;			// 충돌체 저장 포인터
};

END