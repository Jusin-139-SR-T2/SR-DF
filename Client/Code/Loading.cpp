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
	Loading_For_Texture();
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MeshColBufferComp", CMeshColComp::Create(m_pGraphicDev)), E_FAIL);

	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColBufferComp", CTriColorComp::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CubeBufferComp", CCubeBufferComp::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_CalculatorComp", CCalculatorComponent::Create(m_pGraphicDev)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainBufferComp", CTerrainBufferComp::Create(m_pGraphicDev, L"./Resource/Texture/Terrain/Height.bmp", VTXCNTX, VTXCNTZ, VTXITV)), E_FAIL);

	// 콜라이더
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SphereColComp", CSphereColComp::Create(m_pGraphicDev)), E_FAIL);

	// 텍스처 컴포넌트
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TerrainTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Tile", L"22")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_SkyBoxTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_CUBE, L"TestCube", L"4")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_UITextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"UI", L"HudBox_Main")), E_FAIL);
	
	// 플레이어 손
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerLeftTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player_Single", L"Left_RunHand")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_PlayerRightTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Player_Single", L"Right_RunHand")), E_FAIL);

	// 몬스터
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BrownTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Brown_Single", L"Stand_South")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_GrayTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Gray_Single", L"CrotchHit")), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_BossTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Boss_Single", L"CrotchHit")), E_FAIL);
	

	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	m_bFinish = true;

	return 0;
}

// 텍스처 매니저 텍스처 추가
HRESULT CLoading::Loading_For_Texture()
{
#pragma region 기타 등등
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Tile/Tile/22.jpg", TEX_NORMAL, L"Tile", L"22"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/SkyBox/Test4.dds", TEX_CUBE, L"TestCube", L"4"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/UI/PlayerHudBox_Main.png", TEX_NORMAL, L"UI", L"HudBox_Main"), E_FAIL);
#pragma endregion

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
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Player/Steel_Pipe%d.png", TEX_NORMAL, L"Player_Multi", L"Steel_Pipe", _range<_uint>(0U, 4U)), E_FAIL);
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
	
	// Monster Texture - Brown - Single
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Stand/Stand_South.png", TEX_NORMAL, L"Brown_Single", L"Stand_South"), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious"), E_FAIL);

	// Monster Texture - Brown - Multi
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"BasicAttack", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 11U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Pooping/Pooping_%d.png", TEX_NORMAL, L"Brown_Multi", L"Pooping", _range<_uint>(0U, 2U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/GetUp/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Jump/Jump_%d.png", TEX_NORMAL, L"Brown_Multi", L"Jump", _range<_uint>(0U, 6U)), E_FAIL);
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U)), E_FAIL);

	// Monster Texture - Gray - Single
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Gray/Single/CrotchHit.png", TEX_NORMAL, L"Gray_Single", L"CrotchHit"), E_FAIL);
	// Monster Texture - Gray - Multi
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Gray/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Gray_Multi", L"FacePunch", _range<_uint>(0U, 7U)), E_FAIL);

	// Monster Texture - Boss - Single
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Boss/Single/CrotchHit.png", TEX_NORMAL, L"Boss_Single", L"CrotchHit"), E_FAIL);
	// Monster Texture - Boss - Multi
	FAILED_CHECK_RETURN(Engine::Ready_Texture(L"./Resource/Texture/Monster/Boss/Reloading/Reloading_%d.png", TEX_NORMAL, L"Boss_Multi", L"Reloading", _range<_uint>(0U, 17U)), E_FAIL);


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
