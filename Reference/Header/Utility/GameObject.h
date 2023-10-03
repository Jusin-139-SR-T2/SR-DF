#pragma once

#include "Component.h"

#include "Engine_Macro.h"
#include "ProtoMgr.h"

BEGIN(Engine)

enum class EPRIORITY_TYPE : _uint
{
	UPDATE = 0U, LATE, RENDER, SIZE
};
using EPRIORITY = EPRIORITY_TYPE;

/// <summary>
/// 게임에서 움직이는 모든 오브젝트에 대한 베이스 클래스
/// </summary>
class ENGINE_DLL CGameObject : public CBase
{
	DERIVED_CLASS(CBase, CGameObject)
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject();

public:
	virtual void	Free();

public:
	CComponent*		Get_Component(COMPONENTID eID, const _tchar* pComponentTag);

protected:
	HRESULT			Bind_Component(COMPONENTID eID, CComponent*& pComponent, const _tchar* pComponentTag);

	template<typename T>
	T* Set_DefaultComponent(T*& rpComponent, COMPONENTID eID, const _tchar* pComponentTag);

	template <typename T>
	T* Set_DefaultComponent_FromProto(COMPONENTID eID, const _tchar* pComponentTag, const _tchar* pProtoTag);

protected:
	//_ummap<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	_unmap<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9						m_pGraphicDev;

private:
	CComponent*		Find_Component(const _tchar* pComponentTag, COMPONENTID eID);

public:
	virtual HRESULT Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject();
	virtual void	Render_GameObject()	PURE;

protected:
	virtual void	Collide(CGameObject* pDst) {}//PURE;
	virtual void	CollisionEnterd(CGameObject* pDst) {}//PURE;
	virtual void	CollisionExited(CGameObject* pDst) {}//PURE;

public:
	void Compute_ViewZ(const _vec3* pPos);

public:
	GETSET_EX2(CGameObject*, m_pOwner, Owner, GET, SET)

protected:
	CGameObject*				m_pOwner = nullptr;
	_float						m_fViewZ;

public:		// 우선도 관련 GETSET 함수
	GETSET_EX2(_float, m_fPriority[static_cast<_uint>(EPRIORITY::UPDATE)], PriorityUpdate, GET, SET)
	GETSET_EX2(_float, m_fPriority[static_cast<_uint>(EPRIORITY::LATE)], PriorityLate, GET, SET)
	GETSET_EX2(_float, m_fPriority[static_cast<_uint>(EPRIORITY::RENDER)], PriorityRender, GET, SET)

	GETSET_EX2(_bool, m_bUsePriority[static_cast<_uint>(EPRIORITY::UPDATE)], UsePriorityUpdate, GET, SET)
	GETSET_EX2(_bool, m_bUsePriority[static_cast<_uint>(EPRIORITY::LATE)], UsePriorityLate, GET, SET)
	GETSET_EX2(_bool, m_bUsePriority[static_cast<_uint>(EPRIORITY::RENDER)], UsePriorityRender, GET, SET)
		
protected:
	_float			m_fPriority[static_cast<_uint>(EPRIORITY::SIZE)];			// 우선도
	_bool			m_bUsePriority[static_cast<_uint>(EPRIORITY::SIZE)];		// 우선도 사용 여부, 객체 단위

};



template <typename T>
T* CGameObject::Set_DefaultComponent(T*& rpComponent, COMPONENTID eID, const _tchar* pComponentTag)
{
	T* pComponent = new T();
	NULL_CHECK_RETURN(pComponent, nullptr);
	m_mapComponent[eID].emplace(pComponentTag, pComponent);

	return pComponent;
}

template <typename T>
T* CGameObject::Set_DefaultComponent_FromProto(COMPONENTID eID, const _tchar* pComponentTag, const _tchar* pProtoTag)
{
	T* pComponent = dynamic_cast<T*>(CProtoMgr::GetInstance()->Clone_Proto(pProtoTag));
	
	NULL_CHECK_RETURN(pComponent, nullptr);
	bool bSuccess = m_mapComponent[eID].emplace(pComponentTag, pComponent).second;
	// 보안 코드
	if (!bSuccess)
	{
		Safe_Release(pComponent);
		MSG_BOX("Clone Pushback Failed");
		return nullptr;
	}
	else
	{
		// RefCount + 오너 세팅
		dynamic_cast<CComponent*>(pComponent)->Set_Owner(this);
	}

	return pComponent;
}

END