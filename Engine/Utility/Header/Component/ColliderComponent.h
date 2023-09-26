#pragma once

#include "SceneComponent.h"

#include "PhysicsWorld3D.h"

BEGIN(Engine)

class FCollisionPrimitive;

/// <summary>
/// 콜라이더 컴포넌트는 충돌체 정보와 트랜스폼 정보를 저장하는 클래스입니다.
/// </summary>
class ENGINE_DLL CColliderComponent : public CSceneComponent
{
	DERIVED_CLASS(CSceneComponent, CColliderComponent)

protected:
	explicit CColliderComponent();
	explicit CColliderComponent(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CColliderComponent(const CColliderComponent& rhs);
	virtual ~CColliderComponent();

public:
	static	CComponent*			Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CComponent*			Clone();
	virtual void				Free();

public:
	virtual HRESULT	Ready_Collider();
	virtual _int	Update_Component(const _float& fTimeDelta);
	virtual void	LateUpdate_Component() {}

public:		// 트랜스폼 영역, Transform에서 옮겨온 거임
	GETSET_EX2(_vec3, m_vInfo[INFO_RIGHT],	Right,	GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_UP],		Up,		GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_LOOK],	Look,	GET, SET)
	GETSET_EX2(_vec3, m_vInfo[INFO_POS],	Pos,	GET, SET)

	GETSET_EX2(_vec3,	m_vScale,		Scale,			GET, SET)
	GETSET_EX2(_vec3,	m_vAngle,		Angle,			GET, SET)
	GETSET_EX2(_matrix, m_matWorld,		WorldMatrix,	GET, SET)

protected:
	_vec3		m_vInfo[INFO_END];
	_vec3		m_vScale;
	_vec3		m_vAngle;
	_matrix		m_matWorld;


public:		// 충돌체 저장용 포인터
	GETSET_EX2(FCollisionPrimitive*, m_pCollisionShape, Shape, GET, SET)
	
protected:
	FCollisionPrimitive* m_pCollisionShape = nullptr;			// 충돌체 저장 포인터

protected:
	function<void()>	m_fnTest;

protected:
	// 충돌이 발생할 때 불러오는 함수. 충돌이 발생하면 연결된 함수로 다시 신호를 보내줍니다.
	// 충돌중
	virtual void Collide(CColliderComponent* pDst);
	// 충돌 진입, Collide와 함께 발동
	virtual void CollisionEntered(CColliderComponent* pDst);
	// 충돌 끝남, 모든 충돌 체크가 끝나는 시점에 발동
	virtual void CollisionExited();

protected:
	using pair_collider = pair<CColliderComponent*, _bool>;
	// 충돌 발생시 충돌한 객체를 가리키기 위한 리스트, Collide를 통해 매 프레임 조절됩니다.
	list<pair_collider> m_listColliderObject;
};

END