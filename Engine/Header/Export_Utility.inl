// �Ŵ�����Ʈ �Լ�
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

// ������ �Ŵ��� �Լ�
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


// ������ �Լ�
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


// ���� �Ŵ��� 
HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

// ���� �Ŵ���
inline HRESULT	Ready_TextureMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return CTextureMgr::GetInstance()->Ready_Texture(pGraphicDev);
}
inline HRESULT	Ready_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	return CTextureMgr::GetInstance()->Insert_Texture(pFilePath, eType, pTextureKey, pStateKey, iCntRange);
}


// ���� �Ŵ���
HRESULT		Ready_Physics(const _uint iMaxPhysicsWorld3D)
{
	return CPhysicsMgr::GetInstance()->Ready_Physics(iMaxPhysicsWorld3D);
}
void		StartFrame_Physics()
{
	CPhysicsMgr::GetInstance()->StartFrame_Physics();
}
_int		Update_Physics(const Real& fTimeDelta)
{
	return CPhysicsMgr::GetInstance()->Update_Physics(fTimeDelta);
}



// Ű �Ŵ���
inline HRESULT	Ready_KeyMgr()
{
	return CKeyMgr::GetInstance()->Ready_Key();
}
inline void		Update_KeyMgr()
{
	return CKeyMgr::GetInstance()->Update_Key();
}
inline void		LateUpdate_KeyMgr()
{
	return CKeyMgr::GetInstance()->LateUpdate_Key();
}
// ������ �ִ� ��
inline bool		IsKey_Pressing(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Pressing(iKey);
}
inline bool		IsKey_Pressing(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Pressing(iKey);
}
// ���� ��
inline bool		IsKey_Pressed(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Down(iKey);
}
inline bool		IsKey_Pressed(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Down(iKey);
}
// �� ��
inline bool		IsKey_Released(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Up(iKey);
}
inline bool		IsKey_Released(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Up(iKey);
}

// ������ �ִ� ��
bool		IsMouse_Pressing(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Pressing(iMouse);
}
bool		IsMouse_Pressing(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Pressing(iMouse);
}
// ���� ��
bool		IsMouse_Pressed(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Down(iMouse);
}
bool		IsMouse_Pressed(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Down(iMouse);
}
// �� ��
bool		IsMouse_Released(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Up(iMouse);
}
bool		IsMouse_Released(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Up(iMouse);
}



// �̱��� ���� �ڵ�
void			Release_Utility()
{
	CLightMgr::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CPhysicsMgr::GetInstance()->DestroyInstance();
	CTextureMgr::GetInstance()->DestroyInstance();
	CKeyMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
}