#pragma once

#include "GameObject.h"

BEGIN(Engine)

/// <summary>
/// ���̾�� GameObject�� ���������� GameObject�� Update �κ��� �����ϴ� Ŭ������
/// ���� ������ ������ ó���ϴ� ���Ҹ��� �����ϴ� ���� �������� �Ѵ�.
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
	//void		Render_Layer(); �����δ� Renderer���� ��� �������� �����Ѵ�.

private:
	//_ummap<const _tchar*, CGameObject*> m_mapObject;
	_unmap<const _tchar*, CGameObject*> m_mapObject;

public:
	static CLayer* Create();

private:
	virtual void		Free();
};

END