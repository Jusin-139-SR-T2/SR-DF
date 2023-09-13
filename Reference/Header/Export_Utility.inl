// 매니지먼트 함수
CComponent*	Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	return CManagement::GetInstance()->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}
HRESULT		Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagementInstance)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	*ppManagementInstance = pManagement;

	return S_OK;
}
HRESULT		Set_Scene(CScene* pScene)
{
	return CManagement::GetInstance()->Set_Scene(pScene);
}
_int			Update_Scene(const _float& fTimeDelta)
{
	return CManagement::GetInstance()->Update_Scene(fTimeDelta);
}
void			LateUpdate_Scene()
{
	CManagement::GetInstance()->LateUpdate_Scene();
}
void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CManagement::GetInstance()->Render_Scene(pGraphicDev);
}

// 프로토 매니저 함수
HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent)
{
	return CProtoMgr::GetInstance()->Ready_Proto(pProtoTag, pComponent);
}
template <typename T>
T* Clone_Proto(const _tchar* pProtoTag)
{
	return dynamic_cast<T*>(CProtoMgr::GetInstance()->Clone_Proto(pProtoTag));
}
CComponent*		Clone_Proto(const _tchar* pProtoTag, CComponent*& prComponent)
{
	return CProtoMgr::GetInstance()->Clone_Proto(pProtoTag, prComponent);
}


// 렌더러 함수
void		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject)
{
	CRenderer::GetInstance()->Add_RenderGroup(eType, pGameObject);
}
void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev)
{
	CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
}
void		Clear_RenderGroup()
{
	CRenderer::GetInstance()->Clear_RenderGroup();
}


// 싱글톤 해제 코드
void			Release_Utility()
{
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CPhysicsMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
}