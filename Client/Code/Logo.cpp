#include "stdafx.h"
#include "Logo.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include <Player.h>
#include <Monster.h>
#include <Terrain.h>
#include <Stage.h>
#include <DynamicCamera.h>
#include <TitleLogo.h>

CLogo::CLogo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
{
}

CLogo::~CLogo()
{
}

CLogo* CLogo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Logo Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLogo::Free()
{
	Safe_Release(m_pLoading);
	//Safe_Release(m_pLoadingTexture);

	SUPER::Free();
}

HRESULT CLogo::Ready_Scene()
{
	//사운드
	FAILED_CHECK_RETURN(Engine::Ready_SoundDev(), E_FAIL);
	Engine::CSoundMgr::GetInstance()->Play_Sound(L"Ambience_OldTimeyMusic6.mp3", SOUND_BGM, 0.75f);

	FAILED_CHECK_RETURN(Ready_Prototype(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Layer_Environment(L"Environment"), E_FAIL);

	// 로딩 쓰레드 생성하기
	NULL_CHECK_RETURN(m_pLoading = CLoading::Create(m_pGraphicDev, CLoading::LOADING_STAGE), E_FAIL);

	// 로딩 쓰레드 생성하기
	//NULL_CHECK_RETURN(m_pLoadingTexture = CLoading::Create(m_pGraphicDev, CLoading::LOADING_TEXTURE), E_FAIL);

	return S_OK;
}

_int CLogo::Update_Scene(const _float& fTimeDelta)
{
	_int	iExit = SUPER::Update_Scene(fTimeDelta);

	// 로딩이 끝났을 때 엔터를 눌러 스테이지 씬으로 진입할 수 있다.
	if (m_pLoading->Get_Finish())
	{
		if (GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			CScene* pScene = CStage::Create(m_pGraphicDev);
			NULL_CHECK_RETURN(pScene, E_FAIL);

			FAILED_CHECK_RETURN(Engine::Set_Scene(pScene), E_FAIL);

			return 0;
		}
	}

	return iExit;
}

void CLogo::LateUpdate_Scene()
{
	SUPER::LateUpdate_Scene();
}

void CLogo::Render_Scene()
{
	// _DEBUG 용
}

HRESULT CLogo::Ready_Prototype()
{
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"Resource/Texture/Scene/LogoShot.png", TEX_NORMAL, L"UI", L"Logo"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"Resource/Texture/Scene/Title.png", TEX_NORMAL, L"UI", L"Title"), E_FAIL);

	// 프로토타입 인스턴스를 등록한다.
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TransformComp", CTransformComponent::Create(m_pGraphicDev)), E_FAIL);	// 트랜스폼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_RcTexBufferComp", CRcBufferComp::Create(m_pGraphicDev)), E_FAIL);		// 버퍼
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_LogoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Logo")), E_FAIL);	// 텍스처
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TitleBackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"Title")), E_FAIL);	// 텍스처
	
	return S_OK;
}

HRESULT CLogo::Ready_Layer_Environment(const _tchar* pLayerTag)
{
	// 정상
	// 레이어 초기화
	Engine::CLayer* pLayer = nullptr;
	FAILED_CHECK_RETURN(Add_Layer(pLayerTag, pLayer = Engine::CLayer::Create()), E_FAIL);

	/*FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"DynamicCamera", CDynamicCamera::Create(m_pGraphicDev,
		&_vec3(0.f, 0.f, -1.f),
		&_vec3(0.f, 0.f, 1.f),
		&_vec3(0.f, 1.f, 0.f))), E_FAIL);*/

		// 배경생성
	//FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"Logo", CTitleLogo::Create(m_pGraphicDev)), E_FAIL);

	// 배경생성
	FAILED_CHECK_RETURN(pLayer->Add_GameObject(L"BackGround", CBackGround::Create(m_pGraphicDev)), E_FAIL);

	

	return S_OK;
}



