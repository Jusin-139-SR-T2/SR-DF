#pragma once

#include "GameObject.h"

BEGIN(Engine)

/// <summary>
/// 레이어는 GameObject를 저장하지만 GameObject의 Update 부분을 수행하는 클래스로
/// 게임 로직과 물리를 처리하는 역할만을 수행하는 것을 목적으로 한다.
/// </summary>
class ENGINE_DLL CLayer : public CBase
{
	DERIVED_CLASS(CBase, CLayer)
private:
	explicit CLayer();
	virtual ~CLayer();

public:
	CComponent* Get_Component(COMPONENTID eID, const _tchar* pObjTag, const _tchar* pComponentTag);

public:
	HRESULT			Add_GameObject(const _tchar* pObjTag, CGameObject* pGameObject);
	CGameObject*	Get_GameObject(const _tchar* pObjTag);

public:
	HRESULT		Ready_Layer();
	_int		Update_Layer(const _float& fTimeDelta);
	void		LateUpdate_Layer();
	//void		Render_Layer(); 앞으로는 Renderer에서 대신 렌더링을 수행한다.

private:
	//_ummap<const _tchar*, CGameObject*> m_mapObject;
	_unmap<const _tchar*, CGameObject*> m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void		Free();
};

END