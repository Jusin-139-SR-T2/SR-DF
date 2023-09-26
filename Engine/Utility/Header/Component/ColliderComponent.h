#pragma once

#include "SceneComponent.h"

#include "PhysicsWorld3D.h"

BEGIN(Engine)

class FCollisionPrimitive;

/// <summary>
/// �ݶ��̴� ������Ʈ�� �浹ü ������ Ʈ������ ������ �����ϴ� Ŭ�����Դϴ�.
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

public:		// Ʈ������ ����, Transform���� �Űܿ� ����
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


public:		// �浹ü ����� ������
	GETSET_EX2(FCollisionPrimitive*, m_pCollisionShape, Shape, GET, SET)
	
protected:
	FCollisionPrimitive* m_pCollisionShape = nullptr;			// �浹ü ���� ������

protected:
	function<void()>	m_fnTest;

protected:
	// �浹�� �߻��� �� �ҷ����� �Լ�. �浹�� �߻��ϸ� ����� �Լ��� �ٽ� ��ȣ�� �����ݴϴ�.
	// �浹��
	virtual void Collide(CColliderComponent* pDst);
	// �浹 ����, Collide�� �Բ� �ߵ�
	virtual void CollisionEntered(CColliderComponent* pDst);
	// �浹 ����, ��� �浹 üũ�� ������ ������ �ߵ�
	virtual void CollisionExited();

protected:
	using pair_collider = pair<CColliderComponent*, _bool>;
	// �浹 �߻��� �浹�� ��ü�� ����Ű�� ���� ����Ʈ, Collide�� ���� �� ������ �����˴ϴ�.
	list<pair_collider> m_listColliderObject;
};

END