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
	static	CComponent*			Create(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType);
	virtual CComponent*			Clone();
	virtual void				Free();

public:
	PRIVATE virtual HRESULT Ready_Component() { return S_OK; }
	PUBLIC	virtual HRESULT Ready_Component(LPDIRECT3DDEVICE9 pGraphicDev, ECOLLISION eType);
	virtual _int	Update_Component(const _float& fTimeDelta);
	virtual void	LateUpdate_Component() {}
	virtual void	Render_Component() {}

public:		// �浹ü ����� ������
	GETSET_EX2(FCollisionPrimitive*, m_pCollisionShape, Shape, GET, SET)
	
protected:
	FCollisionPrimitive* m_pCollisionShape = nullptr;			// �浹ü ���� ������

public:			// �̺�Ʈ �Լ�
	template <typename Func,
				typename std::enable_if<std::is_member_function_pointer<Func>::value &&
				std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
				void(CGameObject*)>::value, int>::type = 0>
	HRESULT	Set_Collide_Event(Func fn);

	template <typename Func,
				typename std::enable_if<std::is_member_function_pointer<Func>::value &&
				std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
				void(CGameObject*)>::value, int>::type = 0>
	HRESULT	Set_CollisionEntered_Event(Func fn);

	template <typename Func,
				typename std::enable_if<std::is_member_function_pointer<Func>::value &&
				std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
				void(CGameObject*)>::value, int>::type = 0>
	HRESULT	Set_CollisionExited_Event(Func fn);

protected:
	function<void(CGameObject*)>		m_fnCollide;
	function<void(CGameObject*)>		m_fnCollisionEntered;
	function<void(CGameObject*)>		m_fnCollisionExited;

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





template<typename Func,
			typename std::enable_if<std::is_member_function_pointer<Func>::value&&
			std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
			void(CGameObject*)>::value, int>::type>
inline HRESULT CColliderComponent::Set_Collide_Event(Func fn)
{
	m_fnCollide = [this, fn](CGameObject* pDst) { (this->*fn)(pDst); };

	return S_OK;
}

template<typename Func,
			typename std::enable_if<std::is_member_function_pointer<Func>::value&&
			std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
			void(CGameObject*)>::value, int>::type>
inline HRESULT CColliderComponent::Set_CollisionEntered_Event(Func fn)
{
	m_fnCollisionEntered = [this, fn](CGameObject* pDst) { (this->*fn)(pDst); };

	return S_OK;
}

template<typename Func,
			typename std::enable_if<std::is_member_function_pointer<Func>::value&&
			std::is_same<typename std::remove_pointer<typename std::remove_reference<Func>::type>::type,
			void(CGameObject*)>::value, int>::type>
inline HRESULT CColliderComponent::Set_CollisionExited_Event(Func fn)
{
	m_fnCollisionExited = [this, fn](CGameObject* pDst) { (this->*fn)(pDst); };

	return S_OK;
}
