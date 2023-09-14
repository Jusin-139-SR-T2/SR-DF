#pragma once

#include "Component.h"

#include "Engine_Macro.h"
#include "ProtoMgr.h"

BEGIN(Engine)

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
	_unmap<const _tchar*, CComponent*>		m_mapComponent[ID_END];
	LPDIRECT3DDEVICE9						m_pGraphicDev;

private:
	CComponent*		Find_Component(const _tchar* pComponentTag, COMPONENTID eID);

public:
	virtual HRESULT Ready_GameObject();
	virtual _int	Update_GameObject(const _float& fTimeDelta);
	virtual void	LateUpdate_GameObject();
	virtual void	Render_GameObject()	PURE;

public:
	GETSET_EX2(CGameObject*, m_pOwner, Owner, GET, SET)

protected:
	CGameObject*	m_pOwner = nullptr;

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

	return pComponent;
}

END