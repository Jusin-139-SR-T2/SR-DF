#include "stdafx.h"
#include "Header/Loading.h"

#include "Export_System.h"
#include "Export_Utility.h"


CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev)
	, m_bFinish(false)
{
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eID)
{
	ThisClass* pInstance = new ThisClass(pGraphicDev);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Loading Create Failed");
		return nullptr;
	}

	return pInstance;
}

void CLoading::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	DeleteCriticalSection(&m_Crt);

	Safe_Release(m_pGraphicDev);
}

HRESULT CLoading::Ready_Loading(LOADINGID eID)
{
	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	m_eLoadingID = eID;

	return S_OK;
}

_uint CLoading::Loading_For_Stage()
{
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColBufferComp", CTriColorComp::Create(m_pGraphicDev)), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MeshColBufferComp", CMeshColComp::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainBufferComp", CTerrainBufferComp::Create(m_pGraphicDev, L"./Resource/Texture/Terrain/Height.bmp", VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeBufferComp", CCubeBufferComp::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorComp", CCalculatorComponent::Create(m_pGraphicDev)), E_FAIL);

	// 콜라이더
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereColComp", CSphereColComp::Create(m_pGraphicDev)), E_FAIL);

	// 텍스처 컴포넌트
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Tile/Tile/22.jpg")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_CUBE, L"./Resource/Texture/SkyBox/Test4.dds")), E_FAIL);
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTextureComp2", CTextureComponent::Create(m_pGraphicDev, TEX_CUBE, L"./Resource/Texture/SkyBox/skybox2.dds")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MonsterTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Monster/DazedStrip_539x897_%d.png",14)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/UI/PlayerHudBox_Main.png")), E_FAIL);
	
	// 플레이어 손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Left.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Right.png")), E_FAIL);
	
	// Test
	// 왼손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Left.png")), E_FAIL);
	// 오른손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Right.png")), E_FAIL);
	// 쇠파이프
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightSteelPipeTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SteelPipe%d.png", 2)), E_FAIL);
	// 라이터
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftZippoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Zippo_%d.png", 6)), E_FAIL);
	// 플레이어 총
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerAttackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Gun%d.png", 3)), E_FAIL);
	// 플레이어 총 회전
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerAttackTestTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Spin%d.png", 4)), E_FAIL);
	
	//사운드
	FAILED_CHECK_RETURN(Engine::Ready_SoundDev(), E_FAIL);

	Engine::CSoundMgr::GetInstance()->Play_Sound(L"Ambience_OldTimeyMusic6.mp3", SOUND_BGM, 0.75f);

	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	m_bFinish = true;

	return 0;
}

// 텍스처 매니저 텍스처 추가
HRESULT CLoading::Loading_For_Texture()
{
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/PlayerKick_%d.png", TEX_NORMAL, L"Player", L"Kick", _range<_uint>(0U, 3U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Melee_1.png", TEX_NORMAL, L"Player", L"Catch_Mafia"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/IdleBroken.png", TEX_NORMAL, L"Player", L"Broken_Mafia"), E_FAIL);

	m_bFinish = true;

	return S_OK;
}

_uint CLoading::Thread_Main(void * pArg)
{
	CLoading*		pLoading = reinterpret_cast<CLoading*>(pArg);

	_uint iFlag(0);

	EnterCriticalSection(pLoading->Get_Crt());

	switch (pLoading->Get_LoadingID())
	{
	case LOADING_TEXTURE:
		iFlag = pLoading->Loading_For_Texture();
		break;
	case LOADING_STAGE:
		iFlag = pLoading->Loading_For_Stage();
		break;

	case LOADING_BOSS:
		break;

	}

	LeaveCriticalSection(pLoading->Get_Crt());

	//_endthreadex(0);

	return iFlag;
}
