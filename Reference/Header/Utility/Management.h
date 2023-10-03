#pragma once

#include "Scene.h"

BEGIN(Engine)

enum class EMANAGE_SCENE : _uint
{
	// ���� �� ���, �� ��ȯ�� ���� ���� ������ �������ϴ�. �Ϲ����� ���ӿ��� ���ϴ�.
	SINGLE, 
	// ���� ������ �ʰ� �����ִ� ���, ������ ���ϴ�. �޸𸮸� �� ���ϴ�.
	MULTI
};

class ENGINE_DLL CManagement : public CBase
{
	DERIVED_CLASS_SINGLETON(CBase, CManagement)

private:
	explicit CManagement();
	virtual ~CManagement();

public:
	virtual void	Free();

public:
	CComponent*		Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);
	CGameObject*	Get_GameObject(const _tchar* pLayerTag, const _tchar* pObjTag);

public:
	HRESULT			Ready_Management(const EMANAGE_SCENE eType);
	_int			Update_Scene(const _float& fTimeDelta);
	void			LateUpdate_Scene();
	void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	HRESULT			Set_Scene(CScene* pScene);
	HRESULT			Set_Scene(wstring strSceneName);
	HRESULT			Add_Scene(CScene* pScene, wstring strSceneName);
	HRESULT			Clear_Scene();

private:
	CScene*								m_pScene_Current;
	_unmap<wstring, CScene*>			m_mapScene;
	EMANAGE_SCENE						m_eSceneProcess;


};

END