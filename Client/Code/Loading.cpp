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
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/LeftHand0.png")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/RightHand0.png")), E_FAIL);
	
	// Test
	//// 왼손
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Left.png")), E_FAIL);
	//// 오른손
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightHandTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SprintingHands_Right.png")), E_FAIL);
	//// 쇠파이프
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightSteelPipeTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/SteelPipe%d.png", 2)), E_FAIL);
	//// 라이터
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftZippoTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Zippo_%d.png", 6)), E_FAIL);
	//// 플레이어 총
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerAttackTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Gun%d.png", 3)), E_FAIL);
	//// 플레이어 총 회전
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerAttackTestTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"./Resource/Texture/Player/Spin%d.png", 4)), E_FAIL);
	
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
	
#pragma region 싱글 텍스처
	// 왼손 달리기
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftRunHand.png", TEX_NORMAL, L"Player_Single", L"Left_RunHand"), E_FAIL);
	// 오른손 달리기
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightRunHand.png", TEX_NORMAL, L"Player_Single", L"Right_RunHand"), E_FAIL);
	// 왼손 방어
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftBlockHand.png", TEX_NORMAL, L"Player_Single", L"Left_BlockHand"), E_FAIL);
	// 오른손 방어
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightBlockHand.png", TEX_NORMAL, L"Player_Single", L"Right_BlockHand"), E_FAIL);
	// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/OpenHand.png", TEX_NORMAL, L"Player_Single", L"OpenHand"), E_FAIL);
#pragma endregion

#pragma region 멀티 텍스처
	// 왼손
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftHand%d.png", TEX_NORMAL, L"Player_Multi", L"Left_Hand", _range<_uint>(0U, 1U)), E_FAIL);
	// 오른손
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightHand%d.png", TEX_NORMAL, L"Player_Multi", L"Right_Hand", _range<_uint>(0U, 1U)), E_FAIL);
	// 주먹 차징 (오른손)
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightHand_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"Charging", _range<_uint>(0U, 1U)), E_FAIL);
	// 쇠파이프
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Steel_Pipe%d.png", TEX_NORMAL, L"Player_Multi", L"Steel_Pipe", _range<_uint>(0U, 3U)), E_FAIL);
	// 쇠파이프 차징
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Steel_Pipe_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"Steel_Pipe_Charging", _range<_uint>(0U, 4U)), E_FAIL);
	// 프라이팬
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/FryingPan%d.png", TEX_NORMAL, L"Player_Multi", L"FryingPan", _range<_uint>(0U, 4U)), E_FAIL);
	// 프라이팬 차징
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/FryingPan_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"FryingPan_Charging", _range<_uint>(0U, 3U)), E_FAIL);
	// 라이터
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Zippo_%d.png", TEX_NORMAL, L"Player_Multi", L"Righter", _range<_uint>(0U, 5U)), E_FAIL);
	// 플레이어 총
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Gun%d.png", TEX_NORMAL, L"Player_Multi", L"Gun", _range<_uint>(0U, 3U)), E_FAIL);
	// 플레이어 총 회전
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Spin%d.png", TEX_NORMAL, L"Player_Multi", L"Gun_Spin", _range<_uint>(0U, 3U)), E_FAIL);
	// 플레이어 톰슨 기관총
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Thompson%d.png", TEX_NORMAL, L"Player_Multi", L"Thompson", _range<_uint>(0U, 3U)), E_FAIL);
	// 맥주병
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/BeerBottle%d.png", TEX_NORMAL, L"Player_Multi", L"BeerBottle", _range<_uint>(0U, 4U)), E_FAIL);
	// 발차기
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/PlayerKick_%d.png", TEX_NORMAL, L"Player_Multi", L"Kick", _range<_uint>(0U, 3U)), E_FAIL);
#pragma endregion
	
	//// 왼손
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftHand%d.png", TEX_NORMAL, L"Player1", L"Left_Hand", _range<_uint>(0U, 1U)), E_FAIL);
	//// 오른손
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightHand%d.png", TEX_NORMAL, L"Player2", L"Right_Hand", _range<_uint>(0U, 1U)), E_FAIL);
	//// 왼손 달리기
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftRunHand.png", TEX_NORMAL, L"Player3", L"Left_RunHand"), E_FAIL);
	//// 오른손 달리기
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightRunHand.png", TEX_NORMAL, L"Player4", L"Right_RunHand"), E_FAIL);
	//// 왼손 방어
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/LeftBlockHand.png", TEX_NORMAL, L"Player5", L"Left_BlockHand"), E_FAIL);
	//// 오른손 방어
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightBlockHand.png", TEX_NORMAL, L"Player6", L"Right_BlockHand"), E_FAIL);
	//// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/OpenHand.png", TEX_NORMAL, L"Player7", L"OpenHand"), E_FAIL);
	//// 주먹 차징 (오른손)
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/RightHand_Charging%d.png", TEX_NORMAL, L"Player8", L"Charging", _range<_uint>(0U, 1U)), E_FAIL);
	//// 쇠파이프
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Steel_Pipe%d.png", TEX_NORMAL, L"Player9", L"Steel_Pipe", _range<_uint>(0U, 2U)), E_FAIL);
	//// 쇠파이프 차징
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Steel_Pipe_Charging%d.png", TEX_NORMAL, L"Player10", L"Steel_Pipe_Charging", _range<_uint>(0U, 4U)), E_FAIL);
	//// 프라이팬
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/FryingPan%d.png", TEX_NORMAL, L"Player11", L"FryingPan", _range<_uint>(0U, 4U)), E_FAIL);
	//// 프라이팬 차징
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/FryingPan_Charging%d.png", TEX_NORMAL, L"Player12", L"FryingPan", _range<_uint>(0U, 3U)), E_FAIL);
	//// 라이터
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Zippo_%d.png", TEX_NORMAL, L"Player13", L"Righter", _range<_uint>(0U, 5U)), E_FAIL);
	//// 플레이어 총
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Gun%d.png", TEX_NORMAL, L"Player14", L"Gun", _range<_uint>(0U, 2U)), E_FAIL);
	//// 플레이어 총 회전
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Spin%d.png", TEX_NORMAL, L"Player15", L"Gun_Spin", _range<_uint>(0U, 3U)), E_FAIL);
	//// 플레이어 톰슨 기관총
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Thompson%d.png", TEX_NORMAL, L"Player16", L"Thompson", _range<_uint>(0U, 3U)), E_FAIL);
	//// 맥주병
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/BeerBottle%d.png", TEX_NORMAL, L"Player17", L"BeerBottle", _range<_uint>(0U, 4U)), E_FAIL);
	//// 발차기
	//FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/PlayerKick_%d.png", TEX_NORMAL, L"Player18", L"Kick", _range<_uint>(0U, 3U)), E_FAIL);
	
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
