#include "stdafx.h"
#include "Stage.h"
#include "Export_System.h"
#include "Export_Utility.h"

#include "DynamicCamera.h"
#include "BackGround.h"
#include "SkyBox.h"
#include "UI.h"

CStage::CStage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CStage::~CStage()
{

}

HRESULT CStage::Ready_Scene()
{
	FAILED_CHECK_RETURN(Ready_LightInfo(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_SpotLightInfo(), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_GameLogic(L"GameLogic"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Camera(L"Camera"), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_UI(L"UI"), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	return S_OK;
}

_int CStage::Update_Scene(const _float & fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	return iExit;
}

void CStage::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();
}

void CStage::Render_Scene()
{
	// _DEBUG 용

}

HRESULT CStage::Ready_Layer_Environment(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject*		pGameObject = nullptr;

	

	// SkyBox
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"SkyBox", CSkyBox::Create(m_pGraphicDev)), E_FAIL);

	// Terrain
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Terrain", CTerrain::Create(m_pGraphicDev)), E_FAIL);
	
	return S_OK;
}

HRESULT CStage::Ready_Layer_GameLogic(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject*		pGameObject = nullptr;

	// Player
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Player", CPlayer::Create(m_pGraphicDev)), E_FAIL);

	//Monster
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Monster", CMonster::Create(m_pGraphicDev)), E_FAIL);


	return S_OK;
}

HRESULT CStage::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	Engine::CLayer* pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);
	m_mapLayer.insert({ pLayerTag, pLayer });

	Engine::CGameObject* pGameObject = nullptr;

	// DynamicCamera
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 10.f, -10.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f))), E_FAIL);


	return S_OK;
}

HRESULT CStage::Ready_Layer_UI(const _tchar * pLayerTag)
{
	Engine::CLayer*		pLayer = Engine::CLayer::Create();
	NULL_CHECK_RETURN(pLayer, E_FAIL);

	Engine::CGameObject*		pGameObject = nullptr;

	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"UI", CUI::Create(m_pGraphicDev)), E_FAIL);
	
	m_mapLayer.insert({ pLayerTag, pLayer });
	
	return S_OK;
}

HRESULT CStage::Ready_Layer_Completed()
{
	CPlayer* pObj = dynamic_cast<CPlayer*>(Get_GameObject(L"GameLogic", L"Player"));
	NULL_CHECK_RETURN(pObj, E_FAIL);

	CDynamicCamera* pCam = dynamic_cast<CDynamicCamera*>(Get_GameObject(L"Camera", L"DynamicCamera"));
	NULL_CHECK_RETURN(pCam, E_FAIL);

	pCam->Set_Target(pObj);
	pObj->Set_Camera(pCam);

	return S_OK;
}

HRESULT CStage::Ready_LightInfo()
{
	D3DLIGHT9		tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;

	tLightInfo.Diffuse = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Specular = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Ambient = { 1.f, 1.f, 1.f, 1.f };
	tLightInfo.Direction = { 1.f, -1.f, 1.f };
	
	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tLightInfo, 0), E_FAIL);

	return S_OK;
}

HRESULT CStage::Ready_SpotLightInfo()
{
	D3DLIGHT9		tSpotLightInfo;
	ZeroMemory(&tSpotLightInfo, sizeof(D3DLIGHT9));

	tSpotLightInfo.Type = D3DLIGHT_SPOT;
	
	tSpotLightInfo.Diffuse = { 1.f, 0.f, 0.f, 1.f };  //붉은색 조명
	tSpotLightInfo.Specular = { 1.f, 1.f, 1.f, 1.f };
	tSpotLightInfo.Ambient = { 1.f, 1.f, 1.f, 1.f };
	tSpotLightInfo.Direction = { 1.f, 0.f, 1.f };

	FAILED_CHECK_RETURN(Engine::Ready_Light(m_pGraphicDev, &tSpotLightInfo, 1), E_FAIL);

	return S_OK;
}

CStage * CStage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass*	pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Stage Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CStage::Free()
{
	SUPER::Free();
}
