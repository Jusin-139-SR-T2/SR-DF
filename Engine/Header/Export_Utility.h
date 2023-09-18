#pragma once

#include "Management.h"

#include "ProtoMgr.h"
#include "Renderer.h"
#include "PhysicsMgr.h"
#include "Camera.h"

#include "LightMgr.h"
#include "TextureMgr.h"
#include "PhysicsMgr.h"

#include "CalculatorComponent.h"
#include "TriColorComp.h"
#include "RcColorComp.h"
#include "MeshColComp.h"
#include "RcBufferComp.h"
#include "TerrainBufferComp.h"
#include "CameraComponent.h"
#include "CubeBufferComp.h"
#include "ColliderComponent.h"
#include "SphereColComp.h"
#include "TextureComponent.h"
#include "TransformComponent.h"

#include "PHY_Shape.h"
#include "PHY_Sphere.h"


BEGIN(Engine)

// Management
inline CComponent*	Get_Component(COMPONENTID eID, const _tchar* pLayerTag, const _tchar* pObjTag, const _tchar* pComponentTag);

inline HRESULT		Create_Management(LPDIRECT3DDEVICE9 pGraphicDev, CManagement** ppManagementInstance);

inline HRESULT		Set_Scene(CScene* pScene);
inline _int			Update_Scene(const _float& fTimeDelta);
inline void			LateUpdate_Scene();
inline void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);


// ProtoMgr
inline HRESULT			Ready_Proto(const _tchar* pProtoTag, CComponent* pComponent);
template <typename T>	inline T* Clone_Proto(const _tchar* pProtoTag);
// 클론시 저장할 포인터 변수를 받는 오버로딩 함수
inline CComponent*		Clone_Proto(const _tchar* pProtoTag, CComponent*& prComponent);

// Renderer
inline void		Add_RenderGroup(RENDERID eType, CGameObject* pGameObject);
inline void		Render_GameObject(LPDIRECT3DDEVICE9& pGraphicDev);
inline void		Clear_RenderGroup();

// LightMgr
inline HRESULT			Ready_Light(LPDIRECT3DDEVICE9 pGraphicDev,	const D3DLIGHT9 * pLightInfo,	const _uint & iIndex);

// PhysicsMgr


// TextureMgr
inline HRESULT	Ready_TextureMgr(LPDIRECT3DDEVICE9 pGraphicDev);
inline HRESULT	Ready_Texture(const _tchar* pFilePath, TEXTUREID eType, const _tchar* pTextureKey, const _tchar* pStateKey = L"", const _range<_uint>& iCntRange = _range<_uint>(0U, 0U));

inline void			Release_Utility();

#include "Export_Utility.inl"

END