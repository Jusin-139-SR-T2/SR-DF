#pragma once

#include "Management.h"

#include "TriCol.h"
#include "RcCol.h"
#include "RcTex.h"
#include "TerrainTexComponent.h"
#include "CameraComponent.h"

#include "Texture.h"
#include "Transform.h"

#include "ProtoMgr.h"
#include "Renderer.h"

#include "Camera.h"

#include "Calculator.h"

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


inline void			Release_Utility();

#include "Export_Utility.inl"

END