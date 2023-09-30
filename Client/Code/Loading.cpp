#include "stdafx.h"
#include "Header/Loading.h"

#include "Export_System.h"
#include "Export_Utility.h"

#include <mutex>

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

	m_vecAsyncTexture.reserve(100);

	return S_OK;
}

_uint CLoading::Loading_For_Stage()
{
	Loading_For_Texture();
	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_MeshColBufferComp", CMeshColComp::Create(m_pGraphicDev)), E_FAIL);

	//FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_TriColBufferComp", CTriColorComp::Create(m_pGraphicDev)), E_FAIL);
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
	
	// 오브젝트 
	FAILED_CHECK_RETURN(Engine::Ready_Proto(L"Proto_ObjectTextureComp", CTextureComponent::Create(m_pGraphicDev, TEX_NORMAL, L"Food", L"Cola")), E_FAIL);



	FAILED_CHECK_RETURN(Ready_Layer_Completed(), E_FAIL);

	m_bFinish = true;

	return 0;
}

// 텍스처 매니저 텍스처 추가
HRESULT CLoading::Loading_For_Texture()
{
#pragma region 기타 등등
	Load_Texture(L"./Resource/Texture/Tile/Tile/22.jpg", TEX_NORMAL, L"Tile", L"22");
	Load_Texture(L"./Resource/Texture/SkyBox/Test4.dds", TEX_CUBE, L"TestCube", L"4");
	//Load_Texture(L"./Resource/Texture/SkyBox/Skybox.dds", TEX_CUBE, L"TestCube", L"4");
	Load_Texture(L"./Resource/Texture/UI/PlayerHudBox_Main.png", TEX_NORMAL, L"UI", L"HudBox_Main");
#pragma endregion

#pragma region 싱글 텍스처
	// 왼손 달리기
	Load_Texture(L"./Resource/Texture/Player/LeftRunHand.png", TEX_NORMAL, L"Player_Single", L"Left_RunHand");
	// 오른손 달리기
	Load_Texture(L"./Resource/Texture/Player/RightRunHand.png", TEX_NORMAL, L"Player_Single", L"Right_RunHand");
	// 오픈 핸드 [오른손이 무언가를 할 때 아무것도 아닌상태] (왼손)
	Load_Texture(L"./Resource/Texture/Player/OpenHand.png", TEX_NORMAL, L"Player_Single", L"OpenHand");
#pragma endregion
	
#pragma region 멀티 텍스처
	// 왼손
	Load_Texture(L"./Resource/Texture/Player/LeftHand%d.png", TEX_NORMAL, L"Player_Multi", L"Left_Hand", _range<_uint>(0U, 2U));
	// 오른손
	Load_Texture(L"./Resource/Texture/Player/RightHand%d.png", TEX_NORMAL, L"Player_Multi", L"Right_Hand", _range<_uint>(0U, 2U));
	// 주먹 차징 (오른손)
	Load_Texture(L"./Resource/Texture/Player/RightHand_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"RightHand_Charging", _range<_uint>(0U, 1U));
	// 버리기 (오른손)
	Load_Texture(L"./Resource/Texture/Player/UnderThrowRightHand%d.png", TEX_NORMAL, L"Player_Multi", L"UnderThrow_RightHand", _range<_uint>(0U, 0U));
	// 쇠파이프
	Load_Texture(L"./Resource/Texture/Player/Steel_Pipe%d.png", TEX_NORMAL, L"Player_Multi", L"Steel_Pipe", _range<_uint>(0U, 4U));
	// 쇠파이프 차징
	Load_Texture(L"./Resource/Texture/Player/Steel_Pipe_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"Steel_Pipe_Charging", _range<_uint>(0U, 4U));
	// 프라이팬
	Load_Texture(L"./Resource/Texture/Player/FryingPan%d.png", TEX_NORMAL, L"Player_Multi", L"FryingPan", _range<_uint>(0U, 5U));
	// 프라이팬 차징
	Load_Texture(L"./Resource/Texture/Player/FryingPan_Charging%d.png", TEX_NORMAL, L"Player_Multi", L"FryingPan_Charging", _range<_uint>(0U, 3U));
	// 라이터
	Load_Texture(L"./Resource/Texture/Player/Zippo_%d.png", TEX_NORMAL, L"Player_Multi", L"Righter", _range<_uint>(0U, 5U));
	// 플레이어 총
	Load_Texture(L"./Resource/Texture/Player/Gun%d.png", TEX_NORMAL, L"Player_Multi", L"Gun", _range<_uint>(0U, 3U));
	// 플레이어 총 회전
	Load_Texture(L"./Resource/Texture/Player/Spin%d.png", TEX_NORMAL, L"Player_Multi", L"Gun_Spin", _range<_uint>(0U, 3U));
	// 플레이어 톰슨 기관총
	Load_Texture(L"./Resource/Texture/Player/Thompson%d.png", TEX_NORMAL, L"Player_Multi", L"Thompson", _range<_uint>(0U, 3U));
	// 맥주병
	Load_Texture(L"./Resource/Texture/Player/BeerBottle%d.png", TEX_NORMAL, L"Player_Multi", L"BeerBottle", _range<_uint>(0U, 4U));
	// 발차기
	Load_Texture(L"./Resource/Texture/Player/PlayerKick_%d.png", TEX_NORMAL, L"Player_Multi", L"Kick", _range<_uint>(0U, 3U));
#pragma endregion

#pragma region Brown Monster
	// Monster Texture - Brown - Single : 3EA
	Load_Texture(L"./Resource/Texture/Monster/Brown/Stand/Stand_South.png", TEX_NORMAL, L"Brown_Single", L"Stand_South");
	Load_Texture(L"./Resource/Texture/Monster/Brown/Single/Suspicious.png", TEX_NORMAL, L"Brown_Single", L"Suspicious");
	Load_Texture(L"./Resource/Texture/Monster/Brown/Single/CrotchHit.png", TEX_NORMAL, L"Brown_Single", L"CrotchHit");

	// Monster Texture - Brown - Multi : 18EA
	Load_Texture(L"./Resource/Texture/Monster/Brown/BasicAttack/BasicAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"BasicAttack", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/InchForward/InchForward_%d.png", TEX_NORMAL, L"Brown_Multi", L"InchForward", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Brown_Multi", L"HeavyAttack", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Chopped/KarateChopped_%d.png", TEX_NORMAL, L"Brown_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/WalkNorth/WalkNorth_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_North", _range<_uint>(0U, 31U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Brown_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Run/South/RunSouth_%d.png", TEX_NORMAL, L"Brown_Multi", L"RunSouth", _range<_uint>(0U, 20U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Headless/Headless_%d.png", TEX_NORMAL, L"Brown_Multi", L"Headless", _range<_uint>(0U, 15U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Walk/South/Walk_%d.png", TEX_NORMAL, L"Brown_Multi", L"Walk_South", _range<_uint>(0U, 23U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Strafing/Strafing_%d.png", TEX_NORMAL, L"Brown_Multi", L"Strafing", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Falling/Falling_%d.png", TEX_NORMAL, L"Brown_Multi", L"Falling", _range<_uint>(0U, 11U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Pooping/Pooping_%d.png", TEX_NORMAL, L"Brown_Multi", L"Pooping", _range<_uint>(0U, 2U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Rest/IdleReady_%d.png", TEX_NORMAL, L"Brown_Multi", L"Rest", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Dazed/Dazed_%d.png", TEX_NORMAL, L"Brown_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Taunt/Taunt_%d.png", TEX_NORMAL, L"Brown_Multi", L"Taunt", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/GetUp/Getup_%d.png", TEX_NORMAL, L"Brown_Multi", L"GetUp", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Death/Death_%d.png", TEX_NORMAL, L"Brown_Multi", L"Death", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Jump/Jump_%d.png", TEX_NORMAL, L"Brown_Multi", L"Jump", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Brown/Hit/Hit_%d.png", TEX_NORMAL, L"Brown_Multi", L"Hit", _range<_uint>(0U, 5U));
#pragma endregion

#pragma region Gray Monster
	// Monster Texture - Gray - Single : 5EA (2 Pending Approval) 
	Load_Texture(L"./Resource/Texture/Monster/Gray/Single/CrotchHit.png", TEX_NORMAL, L"Gray_Single", L"CrotchHit");
	Load_Texture(L"./Resource/Texture/Monster/Gray/Single/Idle.png", TEX_NORMAL, L"Gray_Single", L"Idle"); // SOUTH 
	//추가 IDLE 은 IDLE 폴더가 따로있음 

	// Monster Texture - Gray - Multi : 21EA
	Load_Texture(L"./Resource/Texture/Monster/Gray/HeavyAttack/HeavyAttack_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeavyAttack", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/UpRightRun/UpRightRun_%d.png", TEX_NORMAL, L"Gray_Multi", L"UpRightRun", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/WalkNorth/WalkNorth_%d.png", TEX_NORMAL, L"Gray_Multi", L"WalkNorth", _range<_uint>(0U, 22U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Gray_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/HeadShot/HeadShot_%d.png", TEX_NORMAL, L"Gray_Multi", L"HeadShot", _range<_uint>(0U, 19U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Frighten/Frighten_%d.png", TEX_NORMAL, L"Gray_Multi", L"Frighten", _range<_uint>(0U, 18U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/SideWalk/SideWalk_%d.png", TEX_NORMAL, L"Gray_Multi", L"SideWalk", _range<_uint>(0U, 6U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Headless/Headless_%d.png", TEX_NORMAL, L"Gray_Multi", L"Coward", _range<_uint>(0U, 21U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Falling/Falling_%d.png", TEX_NORMAL, L"Gray_Multi", L"Falling", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Chopped/Chopped_%d.png", TEX_NORMAL, L"Gray_Multi", L"Chopped", _range<_uint>(0U, 17U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/KeepEye/KeepEye_%d.png", TEX_NORMAL, L"Gray_Multi", L"KeepEye", _range<_uint>(0U, 26U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/YouDie/YouDie_%d.png", TEX_NORMAL, L"Gray_Multi", L"YouDie", _range<_uint>(0U, 11U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Attack/Attack_%d.png", TEX_NORMAL, L"Gray_Multi", L"Attack", _range<_uint>(0U, 8U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Dazed/Dazed_%d.png", TEX_NORMAL, L"Gray_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Taunt/Taunt_%d.png", TEX_NORMAL, L"Gray_Multi", L"Taunt", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Throw/Throw_%d.png", TEX_NORMAL, L"Gray_Multi", L"Throw", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Block/Block_%d.png", TEX_NORMAL, L"Gray_Multi", L"Block", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Death/Death_%d.png", TEX_NORMAL, L"Gray_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Walk/Walk_%d.png", TEX_NORMAL, L"Gray_Multi", L"Walk", _range<_uint>(0U, 18U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Rest/Rest_%d.png", TEX_NORMAL, L"Gray_Multi", L"Rest", _range<_uint>(0U, 4U));
	Load_Texture(L"./Resource/Texture/Monster/Gray/Run/Run_%d.png", TEX_NORMAL, L"Gray_Multi", L"Run", _range<_uint>(0U, 21U));
#pragma endregion

#pragma region Boss Monster
	// Monster Texture - Boss - Single
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Pre_Attack.png", TEX_NORMAL, L"Boss_Single", L"Pre_Attack");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/CrotchHit.png", TEX_NORMAL, L"Boss_Single", L"CrotchHit");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/SideReady.png", TEX_NORMAL, L"Boss_Single", L"SideReady");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/BackIdle.png", TEX_NORMAL, L"Boss_Single", L"BackIdle");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Hit_A.png", TEX_NORMAL, L"Boss_Single", L"Hit_A");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Hit_B.png", TEX_NORMAL, L"Boss_Single", L"Hit_B");
	Load_Texture(L"./Resource/Texture/Monster/Boss/Single/Idle.png", TEX_NORMAL, L"Boss_Single", L"Idle");

	// Monster Texture - Boss - Multi
	Load_Texture(L"./Resource/Texture/Monster/Boss/CloseAttack/CloseAttack_%d.png", TEX_NORMAL, L"Boss_Multi", L"CloseAttack", _range<_uint>(0U, 5U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Reloading/Reloading_%d.png", TEX_NORMAL, L"Boss_Multi", L"Reloading", _range<_uint>(0U, 17U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/IdleReady/IdleReady_%d.png", TEX_NORMAL, L"Boss_Multi", L"IdleReady", _range<_uint>(0U, 15U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/FacePunch/FacePunch_%d.png", TEX_NORMAL, L"Boss_Multi", L"FacePunch", _range<_uint>(0U, 7U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Shooting/Shooting_%d.png", TEX_NORMAL, L"Boss_Multi", L"Shooting", _range<_uint>(0U, 8U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Evasion/Evasion_%d.png", TEX_NORMAL, L"Boss_Multi", L"Evasion", _range<_uint>(0U, 3U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Kicked/Kicked_%d.png", TEX_NORMAL, L"Boss_Multi", L"Kicked", _range<_uint>(0U, 10U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Dazed/Dazed_%d.png", TEX_NORMAL, L"Boss_Multi", L"Dazed", _range<_uint>(0U, 13U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Death/Death_%d.png", TEX_NORMAL, L"Boss_Multi", L"Death", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Running/Run_%d.png", TEX_NORMAL, L"Boss_Multi", L"Run", _range<_uint>(0U, 9U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Roll/Roll_%d.png", TEX_NORMAL, L"Boss_Multi", L"Roll", _range<_uint>(0U, 19U));
	Load_Texture(L"./Resource/Texture/Monster/Boss/Walk/Walk_%d.png", TEX_NORMAL, L"Boss_Multi", L"Walk", _range<_uint>(0U, 28U));
#pragma endregion

#pragma region Object 
	Load_Texture(L"./Resource/Texture/Item/Food/BananaPeel.png", TEX_NORMAL, L"Food", L"BananaPeel");
	Load_Texture(L"./Resource/Texture/Item/Food/EatenApple.png", TEX_NORMAL, L"Food", L"EatenApple");
	Load_Texture(L"./Resource/Texture/Item/Food/Medkit.png", TEX_NORMAL, L"Food", L"Medkit");
	Load_Texture(L"./Resource/Texture/Item/Food/Banana.png", TEX_NORMAL, L"Food", L"Banana");
	Load_Texture(L"./Resource/Texture/Item/Food/Apple.png", TEX_NORMAL, L"Food", L"Apple");
	Load_Texture(L"./Resource/Texture/Item/Food/Cola.png", TEX_NORMAL, L"Food", L"Cola");

	//Load_Texture(L"./Resource/Texture/Item/.png", TEX_NORMAL, L"Food", L"Cola");

	


#pragma endregion

#pragma region Object - Inventory : 직교투영 해야함 

	Load_Texture(L"./Resource/Texture/Item/Inventory/Apple.png", TEX_NORMAL, L"Inventory", L"Apple");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Banana.png", TEX_NORMAL, L"Inventory", L"Banana");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Bottle.png", TEX_NORMAL, L"Inventory", L"Bottle");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Cola.png", TEX_NORMAL, L"Inventory", L"Cola");
	Load_Texture(L"./Resource/Texture/Item/Inventory/FryingPan.png", TEX_NORMAL, L"Inventory", L"FryingPan");
	Load_Texture(L"./Resource/Texture/Item/Inventory/MedKit.png", TEX_NORMAL, L"Inventory", L"MedKit");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Pipe.png", TEX_NORMAL, L"Inventory", L"Pipe");
	Load_Texture(L"./Resource/Texture/Item/Inventory/Revolver.png", TEX_NORMAL, L"Inventory", L"Revolver");
	Load_Texture(L"./Resource/Texture/Item/Inventory/TommyGun.png", TEX_NORMAL, L"Inventory", L"TommyGun");

#pragma endregion

	Wait_LoadTextureAsync();

	return S_OK;
}

void CLoading::Load_TextureAsync(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange)
{
	Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CLoading::Load_Texture(const _tchar* pfilePath, TEXTUREID eID, const _tchar* pGroupName, const _tchar* pTextureName, const _range<_uint>& iCntRange, _bool bAsync)
{
	if (bAsync)
		m_vecAsyncTexture.push_back(async(launch::async, &CLoading::Load_TextureAsync, this, pfilePath, eID, pGroupName, pTextureName, iCntRange));
	else
		Engine::Ready_Texture(pfilePath, eID, pGroupName, pTextureName, iCntRange);
}

void CLoading::Wait_LoadTextureAsync()
{
	for (_uint i = 0; i < m_vecAsyncTexture.size(); i++)
	{
		m_vecAsyncTexture[i].get();
	}
	m_vecAsyncTexture.clear();
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
