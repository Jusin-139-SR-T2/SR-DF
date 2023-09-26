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