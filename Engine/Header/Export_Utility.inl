// 매니지먼트 함수
CComponent*	Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag)
{
	return CManagement::GetInstance()->Get_Component(eID, pLayerTag, pObjTag, pComponentTag);
}
HRESULT		Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagementInstance, EMANAGE_SCENE eType)
{
	CManagement*		pManagement = CManagement::GetInstance();
	NULL_CHECK_RETURN(pManagement, E_FAIL);

	pManagement->Ready_Management(eType);

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
void			Add_GameObject(const _tchar* pLayerTag, CGameObject* const pObj)
{
	CManagement::GetInstance()->Add_GameObject(pLayerTag, pObj);
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
_bool		IsReady_Renderer()
{
	return (CRenderer::GetInstance()->Get_ReadyResult() == S_OK);
}
CRenderer*	Get_Renderer()
{
	return CRenderer::GetInstance();
}
HRESULT		Ready_Renderer(const _uint iWidth, const _uint iHeight)
{
	return CRenderer::GetInstance()->Ready_Renderer(iWidth, iHeight);
}
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


// 조명 매니저 
HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev, const D3DLIGHT9* pLightInfo, const _uint& iIndex)
{
	return CLightMgr::GetInstance()->Ready_Light(pGraphicDev, pLightInfo, iIndex);
}

// 조명 매니저
inline HRESULT	Ready_TextureMgr(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return CTextureMgr::GetInstance()->Ready_Texture(pGraphicDev);
}
inline HRESULT	Ready_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey, const _range<_uint>& iCntRange)
{
	return CTextureMgr::GetInstance()->Insert_Texture(pFilePath, eType, pTextureKey, pStateKey, iCntRange);
}


// 물리 매니저
HRESULT		Ready_PhysicsMgr(const _uint iMaxPhysicsWorld3D)
{
	return CPhysicsMgr::GetInstance()->Ready_Physics(iMaxPhysicsWorld3D);
}
void		StartFrame_PhysicsMgr()
{
	CPhysicsMgr::GetInstance()->StartFrame_Physics();
}
_int		Update_PhysicsMgr(const Real& fTimeDelta)
{
	return CPhysicsMgr::GetInstance()->Update_Physics(fTimeDelta);
}
void		Stop_PhysicsSimulation(const _uint iIndex)
{
	CPhysicsMgr::GetInstance()->Get_World3D(iIndex)->Pause_Simulation();
}
void		Play_PhysicsSimulation(const _uint iIndex)
{
	CPhysicsMgr::GetInstance()->Get_World3D(iIndex)->Play_Simulation();
}
void		Add_ColliderToPhysicsWorld(const _uint iIndex, FCollisionPrimitive* pCollider)
{
	CPhysicsMgr::GetInstance()->Get_World3D(iIndex)->Add_RigidBody(pCollider->pBody);
}
void		Delete_ColliderToPhysicsWorld(const _uint iIndex, FCollisionPrimitive* pCollider)
{
	CPhysicsMgr::GetInstance()->Get_World3D(iIndex)->Delete_RigidBody(pCollider->pBody);
}
inline list<CColliderComponent*>	IntersectTests(const _uint iWorldID, _vec3 vPos, CColliderComponent* pSrc)
{
	return CPhysicsMgr::GetInstance()->IntersectTests(iWorldID, vPos, pSrc);
}
inline list<CColliderComponent*>	IntersectTests_Sphere(const _uint iWorldID, _vec3 vPos, _float fRadius)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Sphere(iWorldID, vPos, fRadius);
}
inline list<CColliderComponent*>	IntersectTests_Box(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Box(iWorldID, vPos, vHalfSize);
}
inline list<CColliderComponent*>	IntersectTests_Capsule(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Capsule(iWorldID, vPos, vNormal, fRadius);
}

inline list<CGameObject*>	IntersectTests_GetGameObject(const _uint iWorldID, _vec3 vPos, CColliderComponent* pSrc)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_GetGameObject(iWorldID, vPos, pSrc);
}
inline list<CGameObject*>	IntersectTests_Sphere_GetGameObject(const _uint iWorldID, _vec3 vPos, _float fRadius)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Sphere_GetGameObject(iWorldID, vPos, fRadius);
}
inline list<CGameObject*>	IntersectTests_Box_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vHalfSize)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Box_GetGameObject(iWorldID, vPos, vHalfSize);
}
inline list<CGameObject*>	IntersectTests_Capsule_GetGameObject(const _uint iWorldID, _vec3 vPos, _vec3 vNormal, _float fRadius)
{
	return CPhysicsMgr::GetInstance()->IntersectTests_Capsule_GetGameObject(iWorldID, vPos, vNormal, fRadius);
}



// 키 매니저
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
// 누르고 있는 중
inline bool		IsKey_Pressing(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Pressing(iKey);
}
inline bool		IsKey_Pressing(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Pressing(iKey);
}
// 누를 때
inline bool		IsKey_Pressed(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Down(iKey);
}
inline bool		IsKey_Pressed(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Down(iKey);
}
// 뗄 때
inline bool		IsKey_Released(const int& iKey)
{
	return CKeyMgr::GetInstance()->Key_Up(iKey);
}
inline bool		IsKey_Released(const int&& iKey)
{
	return CKeyMgr::GetInstance()->Key_Up(iKey);
}

// 누르고 있는 중
bool		IsMouse_Pressing(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Pressing(iMouse);
}
bool		IsMouse_Pressing(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Pressing(iMouse);
}
// 누를 때
bool		IsMouse_Pressed(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Down(iMouse);
}
bool		IsMouse_Pressed(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Down(iMouse);
}
// 뗄 때
bool		IsMouse_Released(const MOUSEKEYSTATE& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Up(iMouse);
}
bool		IsMouse_Released(const MOUSEKEYSTATE&& iMouse)
{
	return CKeyMgr::GetInstance()->Mouse_Up(iMouse);
}



// 싱글톤 해제 코드
void		Release_Utility()
{
	CLightMgr::GetInstance()->DestroyInstance();
	CRenderer::GetInstance()->DestroyInstance();
	CProtoMgr::GetInstance()->DestroyInstance();
	CTextureMgr::GetInstance()->DestroyInstance();
	CKeyMgr::GetInstance()->DestroyInstance();
	CManagement::GetInstance()->DestroyInstance();
	CPhysicsMgr::GetInstance()->DestroyInstance();		// 물리는 마지막에 해제한다.
}